#include "vfs.h"
#include "utils_c.h"
#include "mm.h"
#include "tmpfs.h"
#include "mini_uart.h"
list fs_list = LIST_HEAD_INIT(fs_list);
struct mount *rootfs;

void init_rootfs()
{
    if (fs_register(tmfps_create()))
    {
        uart_send_string("[fs] Error! fail to register tmpfs\n");
    }
    rootfs = kcalloc(sizeof(struct mount));
    struct filesystem *tmpfs = fs_get("tmpfs");
    if (!tmpfs)
    {
        uart_send_string("[fs] Error! fail to get tmpfs\n");
        return;
    }
    rootfs->fs = tmpfs;
    rootfs->fs->setup_mount(rootfs->fs, rootfs);
    uart_send_string("[fs] init rootfs success\n");
}

int fs_register(struct filesystem *fs)
{
    if (!fs_get(fs->name))
    {
        insert_tail(&fs_list, &fs->list);
        return 0;
    }
    return -1;
}

struct filesystem *fs_get(const char *name)
{
    struct filesystem *fs;
    list_for_each_entry(fs, &fs_list, list)
    {
        if (!utils_str_compare(fs->name, name))
        {
            return fs;
        }
    }
    return NULL;
}

int vfs_open(const char *pathname, int flags, struct file **target)
{
    int res = 0;
    struct vnode *target_node = NULL;

    // 1. Lookup pathname
    res = vfs_lookup(pathname, &target_node);
    if (res == -1 && !(flags & O_CREAT)) // can't lookup and without O_CREAT flag
    {
        return -1;
    }

    // 2. Create a new file handle for this vnode if found.
    *target = kcalloc(sizeof(struct file));
    (*target)->vnode = target_node;
    (*target)->flags = flags;
    (*target)->f_ops = target_node->f_ops;
    (*target)->f_pos = 0;

    // 3. Create a new file if O_CREAT is specified in flags and vnode not found
    if (!target_node && (flags & O_CREAT))
    {
        rootfs->root->v_ops->create(rootfs->root, &target_node, pathname);
        (*target)->vnode = target_node;
    }

    // lookup error code shows if file exist or not or other error occurs
    // 4. Return error code if fails
    // TODO

    return 0;
}

int vfs_close(struct file *file)
{
    // 1. release the file handle
    // kfree(file);
    rootfs->root->f_ops->close(file);
    // 2. Return error code if fails
    // TODO
    return 0;
}

int vfs_write(struct file *file, const void *buf, size_t len)
{
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
    return file->vnode->f_ops->write(file, buf, len);
}

int vfs_read(struct file *file, void *buf, size_t len)
{
    return file->vnode->f_ops->read(file, buf, len);
}

int vfs_mkdir(const char *pathname)
{
    // TODO
    return 0;
}
int vfs_mount(const char *target, const char *filesystem)
{
    // TODO
    return 0;
}
int vfs_lookup(const char *pathname, struct vnode **target)
{
    // TODO: multilevel lookip
    // only search the filename without directory
    if (rootfs->root->v_ops->lookup(rootfs->root, target, pathname))
    {
        return -1;
    }
    return 0;
}

void vfs_test()
{
    struct file *f1;
    int res ;
    if (vfs_open("what", 0, &f1))
    {
        uart_send_string("[V] cant't open \n");
    }
    if (!vfs_open("what", O_CREAT, &f1))
    {
        uart_send_string("[V] create the what file\n");
    }
    vfs_close(f1);
    uart_send_string("[V] close the what file\n");
    if (!vfs_open("what", 0, &f1))
    {
        uart_send_string("[V] open the what \n");
    }
    char buf1[10] = "012345678\n";
    char buf2[10] = {0};
    vfs_write(f1, buf1, 8);
    vfs_close(f1);

    vfs_open("what", 0, &f1);
    vfs_read(f1, buf2, 8);
    uart_printf("read buf2 :%s\n", buf2);
    vfs_close(f1);
    //

}