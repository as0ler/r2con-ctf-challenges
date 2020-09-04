/*
 * Some useful links to understand how FUSE and Sodium work
 *
 * https://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/
 * https://github.com/libfuse/libfuse/blob/fuse-2_9_bugfix/include/fuse.h
 * https://libsodium.gitbook.io/doc/secret-key_cryptography/secretbox
 * https://libsodium.gitbook.io/doc/password_hashing/default_phf
 * https://github.com/sodiumoxide/sodiumoxide/issues/116
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <sys/stat.h>

#define FUSE_USE_VERSION 29

// You can also define these in the makefile
//#define DEBUG 1
//#define NOCRYPTO 1

#include <fuse.h>
#include <sodium.h>
#include <pthread.h>

// This will be used later to troll the ctf players :D
// I considered using an ASCII goatse here but it felt a bit too much
static char *surprise =
"                                                ,g88bg,\n"
"                                       ,g8888g, )   \"8b\n"
"                                       )~  \"88b<    CIP\n"
"                                      <    CI8I \\_ ,'|'\n"
"                                       \\_ ,-I88,  |  (\n"
"                                         |  |88b ,'   \\\n"
"                                        ,'   Y8P/ ,'   `,\n"
"                                      ,' ,'   8( ,'     |\n"
"                                     @   ;    |(.;    ) (\n"
"                                     `-,/   ,'( \\(   / ,'\n"
"  Yb,     ________                     /   / ,' /'  /  (\n"
"   Y8baadP\"\"\"\"\"\"\"\"Yba,_             .-'  ,' _|-'  ,'   |\n"
"aaadP\"'             `\"\"Yb,       .-'  _.'--'  _.-'     (\n"
"`Y8(                    `\"Yb, ,-' _.-(___..--'\\ (      \\\n"
"  `Y,                      \"/' ,-'    ,'       )/       )\n"
"    Y,  (O)                `--\"a,  _-'       ,-'        )\n"
"    `Y,                        \"Y-'       ,-'          /dbaa,,____,aa,_\n"
"     `Y,       ,aa            ,'        ,'         _,-'     ``\"\"\"\"''  \"Y,\n"
"       Y,      d'8           /        ,'       _,-'               \"Ya   `Y,\n"
"       `b      8 8          (      ,-(     ,-''                     `Y,   Y,\n"
"        Ya o  ,8 8          (     `-._\\    \"-.                        b   `b\n"
"         Yb,_,dP 8           `-.      `-_     `-.                     Y    8\n"
"          `\"\"\"\"  Y              `-._     \\-._    `-_                  8    8\n"
"                 I,                 `-._  `._`-._   `-._              8    8\n"
"                 `b                     `,   )   ``-,   )             P    [\n"
"                  `b                    ,' ,'      ,' ,'             d'    [\n"
"                   d                    |,'        |,'              ,P     [\n"
"                 ,d'    ,PY,         ,P\"YaaaaaaP\"Ybaaa,,_           d'     [\n"
"                d\"    ,P\"  Y,        d'           8'  `\"\"db,       d'      8\n"
"               d'   ,P\"    `Y,       8            I,     d'\"b,     8a      P\n"
"              d(    (       `Y,      P            `b    ,P  `Y,    8`Ya___d'\n"
"              \"Y,   \"b,      `Y,    ,I             8    d'   `8    8  `\"\"\"'\n"
"                \"Y,   \"b,  __ `8,   d'            ,8   ,P     8    8\n"
"                  \"Y,   \"bd88b `b   8             I'   d'     Y,   8\n"
"                    \"Y,    888b 8   8             8   ,P      `b   8\n"
"           Normand    \"Ya,,d888b8   P            d'  ,P'       8   Y,\n"
"           Veilleux      `\"\"\"\",d\"  ,I        ,adPb__aP'        Y   `b\n"
"                           ,a8P,__aP'       d888888P'         ,d    8\n"
"                          d8888888'         88888888       ,d888bbaaP\n"
"                          88888888                         88888888'\n"
"                                                           \"\"\"\"\"\"\"\"\n";

#pragma pack(1)

#define FS_SIGNATURE 0xcaca
#define FS_MAX_FILES 0x100
#define FS_BLOCK_SIZE 0x1000
typedef struct {
    u_int16_t signature;    // FS_SIGNATURE
    u_int8_t num_files;     // each file is 1 kb, maximum 256 files
    char hashed_password[crypto_pwhash_STRBYTES];       // hashed password we just compare later xDD
    unsigned char key[crypto_secretbox_KEYBYTES];       // hardcoded encryption key xDDD
    unsigned char nonce[crypto_secretbox_NONCEBYTES];   // and also hardcoded nonce xDDDDD
} FS_HEADER;

#define FS_MAX_PATH (256 - crypto_secretbox_MACBYTES - sizeof(u_int16_t))
typedef struct {
    unsigned char mac[crypto_secretbox_MACBYTES];   // detached MAC for each file content
    u_int16_t size;                                 // file size
    unsigned char name[FS_MAX_PATH];                // must be null terminated, padded with nulls
} FS_FILE_ENTRY;

// All encrypted volumes have the exact same size
#define FS_TOTAL_SIZE (sizeof(FS_HEADER) + (sizeof(FS_FILE_ENTRY) * FS_MAX_FILES) + (FS_BLOCK_SIZE * FS_MAX_FILES))

// Macro to calculate the offset for a file entry given a file index
#define FS_GET_ENTRY(index) (sizeof(FS_HEADER) + (sizeof(FS_FILE_ENTRY) * index))

// Macro to calculate the offset for a file data block given a file index
#define FS_GET_BLOCK(index) (sizeof(FS_HEADER) + (sizeof(FS_FILE_ENTRY) * FS_MAX_FILES) + (FS_BLOCK_SIZE * index))

#pragma pack()

// Structure in memory with all we need to decrypt files
typedef struct {
    pthread_mutex_t lock;
    char *password;
    int fd;
    FS_HEADER header;
} FS_STATE;

static FS_STATE global_state; 

void fs_lock() { pthread_mutex_lock(&global_state.lock); }
void fs_unlock() { pthread_mutex_unlock(&global_state.lock); }

ssize_t fs_read_buffer(off_t offset, char *buf, ssize_t count)
{
#ifdef DEBUG
    printf("fs_read_buffer(%016lx, %p, %016lx)\n", offset, buf, count);
#endif
    ssize_t orig_count = count;
    if (orig_count < 0) {
        return -1;
    }
    if (lseek(global_state.fd, offset, SEEK_SET) < 0) {
        return -1;
    }
    while (count > 0) {
        ssize_t num = read(global_state.fd, buf, count);
#ifdef DEBUG
        printf("read(%i, %p, %016lx) -> %016lx\n", global_state.fd, buf, count, num);
#endif
        if (num < 0) {
            return -1;
        }
        count = count - num;
        buf = buf + num;
    }
#ifdef DEBUG
    printf("fs_read_buffer(%016lx, %p, %016lx) -> %016lx\n", offset, buf, count, orig_count);
#endif
    return orig_count;
}

ssize_t fs_write_buffer(off_t offset, char *buf, ssize_t count)
{
#ifdef DEBUG
    printf("fs_write_buffer(%016lx, %p, %016lx)\n", offset, buf, count);
#endif
    ssize_t orig_count = count;
    if (orig_count < 0) {
        return -1;
    }
    if (lseek(global_state.fd, offset, SEEK_SET) < 0) {
        return -1;
    }
    while (count > 0) {
        ssize_t num = write(global_state.fd, buf, count);
#ifdef DEBUG
        printf("write(%i, %p, %016lx) -> %016lx\n", global_state.fd, buf, count, num);
#endif
        if (num < 0) {
            return -1;
        }
        count = count - num;
        buf = buf + num;
    }
#ifdef DEBUG
    printf("fs_write_buffer(%016lx, %p, %016lx) -> %016lx\n", offset, buf, count, orig_count);
#endif
    return orig_count;
}

int fs_encrypt(unsigned char *ciphertext, const unsigned char *plaintext, FS_FILE_ENTRY *entry)
{
    int ret = 0;
#ifdef DEBUG
    printf("Encrypting file %s -> %s\n", entry->name, plaintext);
#endif
#ifdef NOCRYPTO
    memcpy(ciphertext, plaintext, FS_BLOCK_SIZE);
#else
    ret = crypto_secretbox_detached(ciphertext, entry->mac, plaintext, FS_BLOCK_SIZE, global_state.header.nonce, global_state.header.key);
#endif
#ifdef DEBUG
    printf("Encryption status: %i\n", ret);
#endif
    return ret;
}

int fs_decrypt(unsigned char *plaintext, const unsigned char *ciphertext, const FS_FILE_ENTRY *entry)
{
    int ret = 0;
#ifdef NOCRYPTO
    memcpy(plaintext, ciphertext, FS_BLOCK_SIZE);
#else
    ret = crypto_secretbox_open_detached(plaintext, ciphertext, entry->mac, FS_BLOCK_SIZE, global_state.header.nonce, global_state.header.key);
#endif
#ifdef DEBUG
    printf("Decrypting file %s -> %s\n", entry->name, plaintext);
#endif
#ifdef DEBUG
    printf("Decryption status: %i\n", ret);
#endif
    return ret;
}

int fs_validate_path(const char *path)
{
    for (int i = 1; i < strlen(path); i++) {
        if (path[i] == '/') {
            return 0;
        }
    }
    return 1;
}

static void *do_init (struct fuse_conn_info *conn)
{

#ifdef DEBUG
    printf("do_init(%p);\n", conn);
#endif

    // Lock the global state
    fs_lock();

    // If we have a new file, create it
    if (global_state.header.signature != FS_SIGNATURE) {
#ifdef DEBUG
        printf("Creating new volume\n");
#endif
        global_state.header.signature = FS_SIGNATURE;
        global_state.header.num_files = 0;
        if (crypto_pwhash_str(global_state.header.hashed_password, global_state.password, strlen(global_state.password),
                    crypto_pwhash_OPSLIMIT_MIN, crypto_pwhash_MEMLIMIT_MIN) != 0) {
#ifdef DEBUG
            printf("Trolling!\n");
#endif
            global_state.header.signature = 0;
        }
        crypto_secretbox_keygen(global_state.header.key);
        randombytes_buf(global_state.header.nonce, sizeof(global_state.header.nonce));
        fs_write_buffer(0, (char *) &global_state.header, sizeof(global_state.header));
    }

#ifdef DEBUG
    printf("Volume contains %i files\n", global_state.header.num_files);
#endif

    // If we have an existing file, check the password
    // For invalid passwords we flag this in the header (but do not write this to disk)
    // This is also hopefully to make it less obvious since players will be looking
    // for a printf("bad password") instead of this
    if (crypto_pwhash_str_verify(global_state.header.hashed_password, global_state.password, strlen(global_state.password)) != 0) {
#ifdef DEBUG
        printf("Trolling!\n");
#endif
        global_state.header.signature = 0;
    }

    // Unlock the state and return
    fs_unlock();

#ifdef DEBUG
    printf("do_init(%p) -> NULL\n", conn);
#endif

    return NULL;
}

static int do_getattr( const char *path, struct stat *st )
{
    int ret = 0;

#ifdef DEBUG
    printf("do_getattr(%s, %p)\n", path, st);
#endif

    fs_lock();

    // Use dummy values for mostly everything
    st->st_uid = getuid();
    st->st_gid = getgid();
    st->st_atime = time(NULL);
    st->st_mtime = time(NULL);

    // Root directory
    if ( strcmp( path, "/" ) == 0 ) {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2; //http://unix.stackexchange.com/a/101536

    // Everything else is assumed to be a file
    } else {
        st->st_mode = S_IFREG | 0644;
        st->st_nlink = 1;
        st->st_size = 0;        // Size 0 by default

        // Validate the path
        if (!fs_validate_path(path)) {
            ret = -1;
        } else {

            // If we have a bad password, let's troll the user a bit
            if (global_state.header.signature != FS_SIGNATURE) {
                if (strcmp(path, "/surprise.txt") == 0) {
#ifdef DEBUG
                    printf("Trolling!\n");
#endif
                    st->st_size = strlen(surprise);
                }

            // If the file exists, let's try to show the real size
            } else {
                //ret = -1;
                for (int index = 0; index < global_state.header.num_files; index++) {
                    FS_FILE_ENTRY entry;
                    memset(&entry, 0, sizeof(entry));
                    fs_read_buffer(FS_GET_ENTRY(index), (char *) &entry, sizeof(entry));
                    if (strcmp((char *) &entry.name, path) == 0) {
#ifdef DEBUG
                        printf("Found file: %s\n", entry.name);
#endif
                        if (entry.size <= FS_BLOCK_SIZE) {
                            st->st_size = entry.size;
                        } else {
                            st->st_size = FS_BLOCK_SIZE;
                        }
                        ret = 0;
                        break;
                    }
                }
            }
        }
    }

    fs_unlock();

#ifdef DEBUG
    printf("do_getattr(%s, %p) -> %i\n", path, st, ret);
#endif

    return ret;
}

static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{

#ifdef DEBUG
    printf("do_readdir(%s, %p, %p, %016lx, %p)\n", path, buffer, filler, offset, fi);
#endif

    fs_lock();

    // Mandatory for all directories
    filler(buffer, ".", NULL, 0);
    filler(buffer, "..", NULL, 0);

    // We only support the root directory
    if ( strcmp( path, "/" ) == 0 )
    {

        // If we have a bad password, let's troll the user a bit
        if (global_state.header.signature != FS_SIGNATURE) {
#ifdef DEBUG
                printf("Trolling!\n");
#endif
            filler(buffer, "surprise.txt", NULL, 0);

        // Otherwise let's show the real files
        } else {
            for (int index = 0; index < global_state.header.num_files; index++) {
                FS_FILE_ENTRY entry;
                memset(&entry, 0, sizeof(entry));
                fs_read_buffer(FS_GET_ENTRY(index), (char *) &entry, sizeof(entry));
#ifdef DEBUG
                printf("Found file: %s\n", entry.name);
#endif
                if (fs_validate_path(entry.name)) {
                    filler(buffer, &(entry.name[1]), NULL, 0);  // skip the leading /
                }
            }
        }
    }

    fs_unlock();

#ifdef DEBUG
    printf("do_readdir(%s, %p, %p, %016lx, %p) -> 0\n", path, buffer, filler, offset, fi);
#endif

    return 0;
}

static int do_truncate( const char *path, off_t offset )
{
    return 0;
}

static int do_mknod( const char *path, mode_t mode, dev_t rdev )
{
    return 0;
}

static int do_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    int ret = -1;

#ifdef DEBUG
    printf("do_create(%s, %08x, %p)\n", path, mode, fi);
#endif

    // Ignore if the path is not valid before even acquiring the lock
    if (!fs_validate_path(path)) {
#ifdef DEBUG
        printf("do_create(%s, %08x, %p) -> -ENOENT\n", path, mode, fi);
#endif
        return -ENOENT;
    }

    // Ignore if we have a bad password before even acquiring the lock
    if (global_state.header.signature != FS_SIGNATURE) {
#ifdef DEBUG
        printf("Trolling!\n");
        printf("do_create(%s, %08x, %p) -> -ENOENT\n", path, mode, fi);
#endif
        return -ENOENT;
    }

    fs_lock();

    // Let's try to find the file first
    // While we're at it look for an empty slot if there is one
    int found = 0;
    int empty = -1;
    for (int index = 0; index < global_state.header.num_files; index++) {
        FS_FILE_ENTRY entry;
        memset(&entry, 0, sizeof(entry));
        fs_read_buffer(FS_GET_ENTRY(index), (char *) &entry, sizeof(entry));
        if (empty == -1 && entry.name[0] == 0) {
            empty = index;
        }
        if (strcmp((char *) &entry.name, path) == 0) {
            found = 1;
            ret = 0;
#ifdef DEBUG
            printf("Found file: %s\n", entry.name);
#endif
        }
    }

    // If we couldn't find the file, let's see if we have a free slot
    // No need to actually create the file here, we'll do it in do_write()
    if (!found && empty >= 0) {
        ret = 0;
#ifdef DEBUG
        printf("File not found, will be created: %s\n", path);
#endif
    }

    fs_unlock();

#ifdef DEBUG
    printf("do_create(%s, %08x, %p) -> %i\n", path, mode, fi, ret);
#endif

    return ret;
}

static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
    int ret = -1;
    char ciphertext[FS_BLOCK_SIZE];
    char plaintext[FS_BLOCK_SIZE];

    memset(ciphertext, 0, sizeof(ciphertext));
    memset(plaintext, 0, sizeof(plaintext));

#ifdef DEBUG
    printf("do_read(%s, %p, %016lx, %016lx, %p)\n", path, buffer, size, offset, fi);
#endif

    // Ignore if the path is not valid before even acquiring the lock
    if (!fs_validate_path(path)) {
#ifdef DEBUG
        printf("do_read(%s, %p, %016lx, %016lx, %p) -> -1\n", path, buffer, size, offset, fi);
#endif
        return -1;
    }

    fs_lock();

    // If we have a bad password, let's troll the user a bit
    if (global_state.header.signature != FS_SIGNATURE) {
        if (strcmp(path, "/surprise.txt") == 0) {
#ifdef DEBUG
            printf("Trolling!\n");
#endif
            do {
                size_t surprise_size = strlen(surprise);
                if (size == 0) {
                    ret = 0;
                    break;
                }
                if (offset < 0) {
                    break;
                }
                if (offset > surprise_size) {
                    break;
                }
                if (size > surprise_size) {
                    size = surprise_size;
                }
                if (offset + size > surprise_size) {
                    size = surprise_size - offset;
                }
                memcpy(buffer, surprise, size);
                ret = size;
            } while(0);
        }

    // If we have the right password, continue
    } else {

        // Let's look for the file
        // If we don't have the file, error out
        for (int index = 0; index < global_state.header.num_files; index++) {
            FS_FILE_ENTRY entry;
            memset(&entry, 0, sizeof(entry));
            fs_read_buffer(FS_GET_ENTRY(index), (char *) &entry, sizeof(entry));
            if (strcmp((char *) &entry.name, path) == 0) {
#ifdef DEBUG
                printf("Found file: %s\n", entry.name);
#endif

                // Sanitize the parameters
                if (offset < 0) {
                    break;
                }
                if (entry.size > FS_BLOCK_SIZE) {
                    entry.size = FS_BLOCK_SIZE;
                }
                if (offset > entry.size) {
                    break;
                }
                if (size > entry.size) {
                    size = entry.size;
                }
                if (offset + size > entry.size) {
                    size = entry.size - offset;
                }
                if (size > sizeof(ciphertext)) {
                    size = sizeof(ciphertext);
                }
                if (size == 0) {
                    ret = 0;
                    break;
                }

                // Get the entire file data into the internal buffer
                ret = fs_read_buffer(FS_GET_BLOCK(index), ciphertext, sizeof(ciphertext));
                if (ret < 0) {
                    break;
                }

                // Decrypt the data
                fs_decrypt(plaintext, ciphertext, &entry);

                // Copy the fragment we were requested
                memcpy(buffer, plaintext + offset, size);

                // Success!
                ret = size;
                break;
            }
        }

    }

    fs_unlock();

#ifdef DEBUG
    printf("do_read(%s, %s, %016lx, %016lx, %p) -> %i\n", path, buffer, size, offset, fi, ret);
#endif

    return ret;
}

static int do_write( const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
    int ret = -1;
    char ciphertext[FS_BLOCK_SIZE];
    char plaintext[FS_BLOCK_SIZE];

    memset(ciphertext, 0, sizeof(ciphertext));
    memset(plaintext, 0, sizeof(plaintext));

#ifdef DEBUG
    printf("do_write(%s, %p, %016lx, %016lx, %p)\n", path, buffer, size, offset, fi);
#endif

    // Ignore if the path is not valid before even acquiring the lock
    if (!fs_validate_path(path)) {
#ifdef DEBUG
        printf("do_write(%s, %p, %016lx, %016lx, %p) -> -1\n", path, buffer, size, offset, fi);
#endif
        return -1;
    }

    fs_lock();

    // Only work with a good password (couldn't think of a troll for this one)
    if (global_state.header.signature == FS_SIGNATURE) {

        // Let's look for the file
        // While we're at it look for an empty slot if there is one
        int found = 0;
        int empty = -1;
        for (int index = 0; index < global_state.header.num_files; index++) {
            FS_FILE_ENTRY entry;
            memset(&entry, 0, sizeof(entry));
            fs_read_buffer(FS_GET_ENTRY(index), (char *) &entry, sizeof(entry));
            if (empty == -1 && entry.name[0] == 0) {
                empty = index;
#ifdef DEBUG
                printf("Found empty slot: %i\n", empty);
#endif
            }
            if (strcmp((char *) &entry.name, path) == 0) {

                // We found the file
                found = 1;
#ifdef DEBUG
                printf("Found file: %s\n", entry.name);
#endif

                // Sanitize the parameters
                if (offset < 0) {
                    break;
                }
                if (offset >= FS_BLOCK_SIZE) {
                    break;
                }
                if (entry.size > FS_BLOCK_SIZE) {
                    entry.size = FS_BLOCK_SIZE;
                }
                if (size > FS_BLOCK_SIZE) {
                    size = FS_BLOCK_SIZE;
                }
                if (offset + size > FS_BLOCK_SIZE) {
                    size = FS_BLOCK_SIZE - offset;
                }
                if (size > sizeof(ciphertext)) {
                    size = sizeof(ciphertext);
                }
                if (size == 0) {
                    ret = 0;
                    break;
                }

                // Calculate the new file size
                size_t new_file_size = offset + size;
                if (new_file_size < entry.size) {
                    new_file_size = entry.size;
                }

                // Get the entire file data into the internal buffer
                ret = fs_read_buffer(FS_GET_BLOCK(index), ciphertext, sizeof(ciphertext));
                if (ret < 0) {
                    break;
                }

                // Decrypt the data
                fs_decrypt(plaintext, ciphertext, &entry);

                // Modify the data
                memcpy(plaintext, buffer, size);

                // Encrypt the data again
                fs_encrypt(ciphertext, plaintext, &entry);

                // Save the encrypted data
                fs_write_buffer(FS_GET_BLOCK(index), ciphertext, sizeof(ciphertext));

                // Save the updated file entry
                fs_write_buffer(FS_GET_ENTRY(index), (char *) &entry, sizeof(entry));

                // Success!
                ret = size;
                break;
            }
        }

        // We did not find the file and we have a free slot, so let's create a new file
        if (empty < 0) {
            empty = global_state.header.num_files;
            if (empty == FS_MAX_FILES - 1) {
                empty = -1;
            }
        }
        if (!found && empty >= 0) {
            do {
#ifdef DEBUG
                printf("Creating file: %s\n", path);
#endif

                // Sanitize the parameters
                if (offset < 0) {
                    break;
                }
                if (offset >= FS_BLOCK_SIZE) {
                    break;
                }
                if (size > FS_BLOCK_SIZE) {
                    size = FS_BLOCK_SIZE;
                }
                if (offset + size > FS_BLOCK_SIZE) {
                    size = FS_BLOCK_SIZE - offset;
                }
                if (size > sizeof(ciphertext)) {
                    size = sizeof(ciphertext);
                }
                if (size == 0) {
                    ret = 0;
                    break;
                }

                // Create a new file entry
                FS_FILE_ENTRY new_entry;
                memset(&new_entry, 0, sizeof(new_entry));
                new_entry.size = offset + size;
                if (new_entry.size > FS_BLOCK_SIZE) {
                    new_entry.size = FS_BLOCK_SIZE;
                }
                strncpy(new_entry.name, path, sizeof(new_entry.name) - 1);

                // Update the total file count
                global_state.header.num_files++;
                if (global_state.header.num_files == 0) {
                    global_state.header.num_files--;
                }
#ifdef DEBUG
                printf("Volume now has %i files\n", global_state.header.num_files);
#endif

                // Copy the data into the plaintext buffer
                memcpy(plaintext + offset, buffer, size);

                // Encrypt the data
                fs_encrypt(ciphertext, plaintext, &new_entry);

                // Save the encrypted data
                fs_write_buffer(FS_GET_BLOCK(empty), ciphertext, sizeof(ciphertext));

                // Save the updated file entry
                fs_write_buffer(FS_GET_ENTRY(empty), (char *) &new_entry, sizeof(new_entry));

                // Update the main header
                fs_write_buffer(0, (char *) &global_state.header, sizeof(global_state.header));

                // Success!
                ret = size;
                break;
            } while(0);
        }
    }

    fs_unlock();

#ifdef DEBUG
    printf("do_write(%s, %p, %016lx, %016lx, %p) -> %i\n", path, buffer, size, offset, fi, ret);
#endif

    return ret;
}

static struct fuse_operations operations = {
	.init       = do_init,
    .getattr    = do_getattr,
    .readdir    = do_readdir,
    .create     = do_create,
    .truncate   = do_truncate,
    .mknod      = do_mknod,
    .read       = do_read,
    .write      = do_write,
};

// A little red herring for the folks using the "strings" command xDDD
static char *scary_message = "Too many failed password attempts, deleting encrypted volume...";

int main(int argc, char *argv[])
{
    printf("Secure Hidden Invulnerable Transactional File System\n");
    printf("The encrypted filesystem that can NEVER be borken!!!\n");
    printf("........................... Created by John MacAffee\n");
    printf("\n");

    memset(&global_state, 0, sizeof(global_state));
    if (sodium_init() < 0) {
        printf("INTERNAL ERROR\n");
        return 1;
    }

    if (argc < 3 || argc > 4) {
        printf("Usage: %s <mount point> <volume.sfs> [password]\n", argv[0]);
        return 1;
    }

    if (argc == 4) {
        global_state.password = strdup(argv[3]);
    } else {
        global_state.password = strdup(getpass("Enter password: "));
    }

    // This is totally poinless because it will be overwritten later,
    // but let's confuse the players xDDD
    if (crypto_pwhash_str(global_state.header.hashed_password, global_state.password, strlen(global_state.password),
                crypto_pwhash_OPSLIMIT_MIN, crypto_pwhash_MEMLIMIT_MIN) != 0) {
        printf("INTERNAL ERROR\n");
        return 1;
    }

    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    if (pthread_mutex_init(&global_state.lock, &mutex_attr) != 0) {
        printf("INTERNAL ERROR\n");
        return 1;
    }

    global_state.fd = open(argv[2], O_RDWR | O_CREAT | O_SYNC, 0600);
    if (global_state.fd < 0) {
        printf("ERROR: cannot open file: %s\n", argv[2]);
        return errno;
    }

    struct flock fl;
    fl.l_type   = F_WRLCK;  /* read/write lock */
    fl.l_whence = SEEK_SET; /* beginning of file */
    fl.l_start  = 0;        /* offset from l_whence */
    fl.l_len    = 0;        /* length, 0 = to EOF */
    fl.l_pid    = getpid(); /* PID */
    fcntl(global_state.fd, F_SETLKW, &fl);

    off_t size = lseek(global_state.fd, 0, SEEK_END);
    if (size == 0) {
        printf("Creating new volume: %s ...\n", argv[2]);
        if (ftruncate(global_state.fd, FS_TOTAL_SIZE) < 0) {
            printf("Insufficient disk space for new volume!\n");
            fl.l_type   = F_UNLCK;
            fcntl(global_state.fd, F_SETLK, &fl); /* unset lock */
            close(global_state.fd);
            unlink(argv[2]);
            return errno;
        }

    } else if (size == FS_TOTAL_SIZE) {
        printf("Opening existing volume: %s\n", argv[2]);
        if (
                fs_read_buffer(0, (char *) &global_state.header, sizeof(global_state.header)) < 0 ||
                global_state.header.signature != FS_SIGNATURE
            ) {
            printf("Invalid volume format or invalid password, aborting!\n");   // "invalid password" here is a red herring
            fl.l_type   = F_UNLCK;
            fcntl(global_state.fd, F_SETLK, &fl); /* unset lock */
            close(global_state.fd);
            return errno;
        }

    } else {
        printf("Invalid volume format: %s\n", argv[2]);
        fl.l_type   = F_UNLCK;
        fcntl(global_state.fd, F_SETLK, &fl); /* unset lock */
        close(global_state.fd);
        return 1;
    }

    printf("Mounting volume at: %s\n", argv[1]);
    char *fake_argv[] = {
        argv[0],
        "-f",
        argv[1],
    };
    return fuse_main(sizeof(fake_argv) / sizeof(fake_argv[0]), (char **) &fake_argv, &operations, NULL);
}
