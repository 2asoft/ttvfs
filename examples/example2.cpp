
/* ttvfs example #2 - init, mounting and basic tree operations */

#include <iostream>
#include <ttvfs.h>

ttvfs::VFSHelper vfs;

static void PrintFile(const char *fn)
{
    ttvfs::File *vf = vfs.GetFile(fn);
    std::cout << "Open file " << fn << ": ";
    if(vf)
    {
        vf->open("r"); // force text mode
        size_t sz = (size_t)vf->size();
        std::string s;
        s.resize(sz);
        size_t bytes = vf->read(&s[0], sz); // read all
        vf->close();

        std::cout << s << std::endl;
    }
    else
    {
        std::cout << "FILE OPEN ERROR!" << std::endl;
    }
}

static void FileCallback(ttvfs::File *vf, void *user)
{
    std::cout << "File: " << vf->name() << " --> " << vf->fullname() << std::endl;

    // Known to be an int ptr.
    unsigned int *c = (unsigned int*)user;
    ++(*c);
}

int main(int argc, char *argv[])
{
    // this should be checked, especially in larger projects
    if(!ttvfs::checkCompat())
    {
        std::cout << "HUH? ttvfs was compiled with different options than this file!" << std::endl;
        return 1;
    }

    vfs.AddLoader(new ttvfs::DiskLoader);

    PrintFile("myfile.txt"); // this is the default file
    PrintFile("patches/myfile.txt"); // this is the patch file

    std::cout << "-- Mounting 'patches' -> ''" << std::endl;

    // merge "patches" into root dir

    vfs.Mount("patches", ""); // <-- this is the better way.
    // all files and subdirs that were in "patches" are now mirrored in "" as well.

    PrintFile("myfile.txt"); // Access the file as before -> it got replaced.

    return 0;//'#####################

    std::cout << "-- Before mounting 'more/even_more/deep' -> 'far' (should error)" << std::endl;

    PrintFile("far/file.txt"); // not found!

    // remount a directory under a different name
    vfs.Mount("more/even_more/deep", "far");

    std::cout << "-- After mounting 'more/even_more/deep' -> 'far'" << std::endl;

    PrintFile("far/file.txt"); // ... and access this file normally

    // mount an external directory (this could be ~/.MyApp or anything)
    vfs.MountExternalPath("../ttvfs", "ext");

    ttvfs::DirBase *ext = vfs.GetDir("ext");
    if(ext)
    {
        //VFS_GUARD(ext); // in case this would be called from multiple threads, lock this directory.

        std::cout << "Listing files in 'ext' subdir ..." << std::endl;

        unsigned int c = 0;
        ext->forEachFile(FileCallback, &c);

        std::cout << c << " files in total!" << std::endl;
    }
    
    return 0;
}
