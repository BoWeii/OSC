#include "vfs.h"
#include "utils_c.h"
#include "mm.h"
#include "tmpfs.h"
#include "mini_uart.h"

list fs_list = LIST_HEAD_INIT(fs_list);
struct mount *rootfs;

const char *next_lvl_path(const char *src, char *dst, int size)
{
    for (int i = 0; i < size; ++i)
    {
        if (src[i] == 0)
        {
            dst[i] = 0;
            return 0;
        }
        else if (src[i] == '/')
        {
            dst[i] = 0;
            return src + i + 1;
        }
        else
            dst[i] = src[i];
    }
    return 0;
}

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
    rootfs->root = vnode_create("", S_IFDIR);
    rootfs->fs->setup_mount(rootfs->fs, rootfs);
#ifdef FS_DEBUG
    uart_send_string("[fs] init rootfs success\n");
#endif
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

    res = vfs_lookup(pathname, &target_node);

    if (res == -1 && !(flags & O_CREAT)) // can't lookup and without O_CREAT flag
    {
        uart_printf("[vfs_open] fail to open the file\n");
        return -1;
    }

    *target = kcalloc(sizeof(struct file));
    (*target)->flags = flags;
    (*target)->f_ops = target_node->f_ops;
    (*target)->f_pos = 0;

    if (!res)
    {
        (*target)->vnode = target_node;
#ifdef FS_DEBUG
        uart_printf("[vfs_open] find it by lookup\n");
#endif
        return 0;
    }

    char prefix[COMPONENT_SIZE] = {0};

    const char *_pathname = &pathname[1]; // pathname=/dir1/dir2/file   _pathname=dir1/dir2/file;
    struct vnode *itr = rootfs->root;
    while (1)
    {
        _pathname = next_lvl_path(_pathname, prefix, COMPONENT_SIZE);
        if (itr->v_ops->lookup(itr, &target_node, prefix) == -1)
        {
            if (!_pathname)
            { // file
                itr->v_ops->create(itr, &target_node, prefix);
                break;
            }
            else
            { // dir
                uart_printf("[vfs_open] No such a directory exist\n");
                return -1;
            }
        }
        else
        {
            if (S_ISDIR(target_node->f_mode))
            {
                itr = target_node;
            }
            else if (S_ISREG(target_node->f_mode))
            {
                break; // find it
            }
        }
    }
    (*target)->vnode = target_node;
    return 0;
}

int vfs_close(struct file *file)
{
    // TODO
    return file->vnode->f_ops->close(file);
}

int vfs_write(struct file *file, const void *buf, size_t len)
{
    return file->vnode->f_ops->write(file, buf, len);
}

int vfs_read(struct file *file, void *buf, size_t len)
{
    return file->vnode->f_ops->read(file, buf, len);
}

int vfs_mkdir(const char *pathname)
{
    struct vnode *target_node = NULL;
    char prefix[COMPONENT_SIZE] = {0};

    const char *_pathname = &pathname[1]; // pathname=/dir1/dir2/file   _pathname=dir1/dir2/file;
    struct vnode *itr = rootfs->root;

    while (1)
    {
        _pathname = next_lvl_path(_pathname, prefix, COMPONENT_SIZE);
        if (itr->v_ops->lookup(itr, &target_node, prefix) == -1)
        { // not found
            if (!_pathname)
            { // encounter end
                itr->v_ops->mkdir(itr, &target_node, prefix);
                itr = target_node;
                return 0;
            }
            uart_printf("[vfs_mkdir] No such a directory exist during traversing\n");
            return -1;
        }
        else
        { //  found
            if (S_ISDIR(target_node->f_mode) && !_pathname)
            {
                uart_printf("[vfs_mkdir] the %s is already exist\n",pathname);
                return -1;
            }
            else if (S_ISDIR(target_node->f_mode))
            {
                itr = target_node;
            }
        }
    }
    return 0;
}
int vfs_mount(const char *target, const char *filesystem)
{
    struct filesystem *fs = fs_get(filesystem);
    if (!fs)
    {
        uart_send_string("[vfs_mount] Error! fail to get fs\n");
        return -1;
    }

    struct vnode *vnode;
    if (vfs_lookup(target, &vnode) == -1)
    {
        uart_send_string("[vfs_mount] Error! fail to lookup\n");
        return -1;
    }

    if (!S_ISDIR(vnode->f_mode))
    {
        uart_send_string("[vfs_mount] Error! the target is not a directory\n");
        return -1;
    }

    struct mount *new_mount = kcalloc(sizeof(struct mount));
    new_mount->fs = fs;
    new_mount->root = vnode;
    new_mount->fs->setup_mount(new_mount->fs, new_mount);

    return 0;
}
int vfs_lookup(const char *pathname, struct vnode **target)
{
    // TODO: handle relative path

    struct vnode *target_node = NULL;

    char prefix[COMPONENT_SIZE] = {0};
    const char *_pathname = &pathname[1]; // pathname=/dir1/dir2/file   _pathname=dir1/dir2/file;
    struct vnode *itr = rootfs->root;

    while (1)
    {
        _pathname = next_lvl_path(_pathname, prefix, COMPONENT_SIZE);
        if (itr->v_ops->lookup(itr, &target_node, prefix) == -1)
        {
            return -1;
        }
        else
        {
            if (S_ISDIR(target_node->f_mode))
            {
                if (!_pathname)
                {
                    *target = target_node; // find the directory
#ifdef FS_DEBUG
                    uart_printf("[vfs_lookup] find the dir\n");
#endif
                    return 0;
                }
                itr = target_node;
            }
            else if (S_ISREG(target_node->f_mode))
            {
                *target = target_node; // find the file
#ifdef FS_DEBUG
                uart_printf("[vfs_lookup] find the file\n");
#endif
                return 0;
            }
        }
    }

    return 0;
}

void vfs_test()
{
    struct file *f1;

    vfs_open("/dir1/dir2/text", O_CREAT, &f1);
    vfs_mkdir("/dir1/dir2");
    vfs_mkdir("/dir1");
    vfs_mkdir("/dir1");
    vfs_mkdir("/dir1/dir1");
    vfs_mkdir("/dir1/dir2");

    if (!vfs_open("/dir1/dir2/text", O_CREAT, &f1))
    {
        uart_send_string("[v] open the /dir1/dir2/text \n");
    }
    char buf1[10] = "012345678\n";
    char buf2[10] = {0};
    vfs_write(f1, buf1, 8);
    vfs_close(f1);

    vfs_open("/dir1/dir2/text", 0, &f1);
    vfs_read(f1, buf2, 8);
    uart_printf("read buf2 :%s\n", buf2);
    vfs_close(f1);

    if (vfs_mkdir("/dir1/dir2/dir3"))
    {
        uart_send_string("[v] vfs_mkdir /dir1/dir2/dir3 fail \n");
    }
    if (!vfs_mount("/dir1/dir2/dir3", "tmpfs"))
    {
        uart_send_string("[v] vfs_mount /dir1/dir2/dir3 success \n");
    }
    
}