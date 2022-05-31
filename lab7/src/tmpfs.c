#include "tmpfs.h"
#include "vfs.h"
#include "utils_c.h"
#include "mm.h"
#include "stat.h"

static int lookup(struct vnode *dir_node, struct vnode **target, const char *component_name);
static int create(struct vnode *dir_node, struct vnode **target, const char *component_name);
static int mkdir(struct vnode *dir_node, struct vnode **target, const char *component_name);
struct vnode_operations tmpfs_v_ops = {
    lookup,
    create,
    mkdir,
};

static int write(struct file *file, const void *buf, size_t len);
static int read(struct file *file, void *buf, size_t len);
static int open(struct vnode *file_node, struct file **target);
static int close(struct file *file);
static long lseek64(struct file *file, long offset, int whence);
struct file_operations tmpfs_f_ops = {
    write,
    read,
    open,
    close,
    lseek64,
};

int setup_mount(struct filesystem *fs, struct mount *mount)
{
    mount->root = vnode_create("/", S_IFDIR);
    mount->root->mount = mount;
    mount->root->f_ops = &tmpfs_f_ops;
    mount->root->v_ops = &tmpfs_v_ops;
    mount->fs = fs;
    return 0;
}

static int lookup(struct vnode *dir_node, struct vnode **target, const char *component_name)
{
    struct vnode *vnode = NULL;
    list_for_each_entry(vnode, &dir_node->childs, self)
    {
        if (!(utils_str_compare(vnode->name, component_name)))
        {
            *target = vnode;
            return 0;
        }
    }
    return -1;
}
static int create(struct vnode *dir_node, struct vnode **target, const char *component_name)
{
    struct vnode *new_vnode = vnode_create(component_name, S_IFREG);
    new_vnode->mount = dir_node->mount;
    new_vnode->v_ops = dir_node->v_ops;
    new_vnode->f_ops = dir_node->f_ops;

    insert_tail(&dir_node->childs, &new_vnode->self);
    dir_node->child_num += 1;

    *target = new_vnode;
    return 0;
}
static int mkdir(struct vnode *dir_node, struct vnode **target, const char *component_name)
{
    // TODO
    return 0;
}

static int write(struct file *file, const void *buf, size_t len)
{
    struct vnode *vnode = file->vnode;
    if (vnode->content_size <= file->f_pos + len)
    { // enlarge content, +1 for EOF
        void *new_content = kcalloc(sizeof(file->f_ops + len + 1));
        memcpy(new_content, vnode->content, vnode->content_size); // origin data;
        kfree(vnode->content);

        vnode->content = new_content;
        vnode->content_size = file->f_pos + len + 1; // pos=22  len=8  30
    }

    memcpy(vnode->content + file->f_pos, buf, len);
    file->f_pos += len;

    return 0;
}
static int read(struct file *file, void *buf, size_t len)
{
    // 1. read min(len, readable size) byte to buf from the opened file.
    // 2. block if nothing to read for FIFO type
    // 3. return read size or error code if an error occurs.
    struct vnode *vnode = file->vnode;

    int min = (len > vnode->content_size - file->f_pos - 1) ? vnode->content_size - file->f_pos - 1 : len; // -1 for EOF;
    if (min == 0)
    {
        return -1; // f_pos at EOF or len==0;
    }
    memcpy(buf, vnode->content + file->f_pos, min);
    file->f_pos += min;
    return min;
}
static int open(struct vnode *file_node, struct file **target)
{
    // TODO
    return 0;
}
static int close(struct file *file)
{
    kfree(file);
    return 0;
}
static long lseek64(struct file *file, long offset, int whence)
{
    // TODO
    return 0;
}

struct filesystem *tmfps_create()
{
    struct filesystem *fs = kmalloc(sizeof(struct filesystem));
    fs->name = "tmpfs";
    fs->setup_mount = &setup_mount;
    list_init(&fs->list);
    return fs;
}

struct vnode *vnode_create(const char *name, unsigned int flags)
{
    struct vnode *vnode = kcalloc(sizeof(struct vnode));

    list_init(&vnode->childs);
    list_init(&vnode->self);
    vnode->child_num = 0;

    size_t name_len = utils_strlen(name);
    vnode->name = kcalloc(sizeof(name_len));
    memcpy(vnode->name, name, name_len);

    vnode->f_mode = flags;

    vnode->content = NULL;
    vnode->content_size = 0;

    return vnode;
}
