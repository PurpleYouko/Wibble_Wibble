/** 
 * @file zz_vfs_pkg.cpp
 * @brief wrapper for TriggerVFS
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    27-nov-2003
 *
 * $Header: /engine/src/zz_vfs_pkg.cpp 29    04-10-12 6:16p Zho $
 * $History: zz_vfs_pkg.cpp $
 * 
 * *****************  Version 29  *****************
 * User: Zho          Date: 04-10-12   Time: 6:16p
 * Updated in $/engine/src
 * 
 * *****************  Version 28  *****************
 * User: Zho          Date: 04-07-01   Time: 11:48a
 * Updated in $/engine/src
 * 7.1.4526
 * 
 * *****************  Version 27  *****************
 * User: Zho          Date: 04-05-19   Time: 8:15p
 * Updated in $/engine/src
 * 
 * *****************  Version 26  *****************
 * User: Zho          Date: 04-04-15   Time: 11:22a
 * Updated in $/engine/src
 * 
 * *****************  Version 25  *****************
 * User: Zho          Date: 04-03-31   Time: 7:22p
 * Updated in $/engine/src
 * 
 * *****************  Version 24  *****************
 * User: Zho          Date: 04-03-22   Time: 3:09p
 * Updated in $/engine/src
 * open with "mr"
 * 
 * *****************  Version 23  *****************
 * User: Zho          Date: 04-03-10   Time: 10:42a
 * Updated in $/engine/src
 * 
 * *****************  Version 22  *****************
 * User: Zho          Date: 04-03-08   Time: 7:52p
 * Updated in $/engine/src
 * To use vfsgetdata()
 * 
 * *****************  Version 21  *****************
 * User: Zho          Date: 04-02-27   Time: 9:45p
 * Updated in $/engine/src
 * gamma ramp bug fixed.
 * 
 * *****************  Version 20  *****************
 * User: Zho          Date: 04-02-27   Time: 6:16p
 * Updated in $/engine/src
 * Separated worker thread's TriggerVFS handle and main thread's
 * TriggerVFS handle because of file reading bug.
 * 
 * *****************  Version 19  *****************
 * User: Zho          Date: 04-02-25   Time: 11:03a
 * Updated in $/engine/src
 * More defensive style.
 * 
 * *****************  Version 18  *****************
 * User: Zho          Date: 04-02-24   Time: 4:26p
 * Updated in $/engine/src
 * 
 * *****************  Version 17  *****************
 * User: Zho          Date: 04-02-21   Time: 6:46p
 * Updated in $/engine/src
 * 
 * *****************  Version 16  *****************
 * User: Zho          Date: 04-02-17   Time: 2:16p
 * Updated in $/engine/src
 * 
 * *****************  Version 15  *****************
 * User: Zho          Date: 04-02-12   Time: 7:09p
 * Updated in $/engine/src
 * Reverted to old locking mechanism that locks/unlocks at every read()
 * call.
 * 
 * *****************  Version 14  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 13  *****************
 * User: Zho          Date: 04-02-10   Time: 9:40p
 * Updated in $/engine/src
 * impreved vfs_pkg read performance.
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-02-01   Time: 5:37p
 * Updated in $/engine/src
 * Added file size(-1) chek.
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-01-12   Time: 4:51p
 * Updated in $/engine/src
 * Added macro to ignore TriggerVFS
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 03-12-09   Time: 7:42p
 * Updated in $/engine/src
 * added thread-based texture loading and acceleration-based camera
 * following system(buggable)
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-12-06   Time: 3:05p
 * Updated in $/engine/src
 * profiler added
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-12-05   Time: 11:19a
 * Updated in $/engine/src
 * let some functions inline
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 03-12-02   Time: 9:20p
 * Updated in $/engine/src
 * do not use zz_vfs_pkg as defaulst filesystem 
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-12-02   Time: 6:03p
 * Updated in $/engine/src
 * read_uint32(int&) bug fixed. if read_uint32(int&) read zero, then
 * status changed to EOF. that was the bug.
 * In zz_type.h, new define zz_vfs added.
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-02   Time: 2:48a
 * Updated in $/engine/src
 * profiling
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-02   Time: 12:07a
 * Updated in $/engine/src
 * after reading, set status to EOF
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-01   Time: 4:11p
 * Updated in $/engine/src
 * some code cleanup
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-01   Time: 12:26p
 * Updated in $/engine/src
 * some updates
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:49p
 * Created in $/engine/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:17p
 * Created in $/Engine/SRC
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-11-28   Time: 6:33p
 * Updated in $/znzin11/engine/source
 * fixed zz_script_lua.cpp parameter passing error. Do not compile
 * zz_script_lua.cpp with /O2 option. Why? I have no idea...
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-28   Time: 1:47a
 * Created in $/znzin11/engine/source
 * new zz_vfs_pkg class added and some modifications
 */

#include "zz_tier0.h"
#include "zz_vfs_pkg.h"
#include "zz_profiler.h"
#include "zz_autolock.h"
#include "zz_system.h"

#include <sys/stat.h>
#include <string>
#include "..\..\..\..\Server\Util\classHASH.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <io.h> // for _access

#ifndef ZZ_IGNORE_TRIGGERVFS

#define MAX_FILESIZE 100000000

//--------------------------------------------------------------------------------
// zz_vfs_pkg_system:
//--------------------------------------------------------------------------------
zz_vfs_pkg_system::zz_vfs_pkg_system() :
	fsystem_(NULL)
{
}

zz_vfs_pkg_system::~zz_vfs_pkg_system()
{
	close_filesystem();
}

bool zz_vfs_pkg_system::set_filesystem (VHANDLE fsystem_in)
{
	assert(!fsystem_in);

	if (fsystem_) {
		close_filesystem();
	}

	fsystem_ = fsystem_in;

	if (!fsystem_) {
		ZZ_LOG("vfs_pkg_system: set_filesystem() failed. invalid fsystem_.\n");
		return false;
	}
	return true;
}

bool zz_vfs_pkg_system::open_filesystem (const char * filesystem_name_in)
{
	assert(filesystem_name_in);

	if (filesystem_name_in[0] == '\0') {
		ZZ_LOG("vfs_pkg_system: open_filesystem(%s) failed. not a valid name.\n", filesystem_name_in);
		return false;
	}

	assert(!fsystem_);
	if (fsystem_) {
		ZZ_LOG("vfs_pkg_system: open_filesystem(%s) failed. already opened\n", filesystem_name_in);
		close_filesystem();
	}
	//ZZ_LOG("vfs_pkg: open_filesystem(%s)\n", filesystem_name);

	zz_slash_converter filesystem_name(filesystem_name_in);

	fsystem_ = OpenVFS(filesystem_name, "mr"); // memory-mapped io read

	assert(fsystem_);

	if (!fsystem_) {
		ZZ_LOG("vfs_pkg_system: open_filesystem(%s) failed. cannot open.\n", filesystem_name);
		return false;
	}
	return true;
}

bool zz_vfs_pkg_system::close_filesystem ()
{
	//ZZ_LOG("vfs_pkg: close_filesystem:\n");

	assert(fsystem_);

	if (!fsystem_) {
		// already closed or not opened
		return true;
	}

	CloseVFS(fsystem_);
	fsystem_ = NULL;

	return true;
}

//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
// zz_vfs_pkg:
//--------------------------------------------------------------------------------
zz_vfs_pkg::zz_vfs_pkg(zz_vfs_pkg_system * pkg_system_in) :
	zz_vfs(),
	fp_(NULL),
	pkg_system_(pkg_system_in)
{
	if (pkg_system_in == NULL) {
		pkg_system_ = zz_system::get_pkg_system();
	}
	set_real_filesystem(this);
}

zz_vfs_pkg::~zz_vfs_pkg(void)
{
	if (fp_) {
		close();
	}
	set_real_filesystem(NULL);
}

bool zz_vfs_pkg::open (const char * filename_in, const zz_vfs_mode mode)
{
	if(mode==ZZ_VFS_WRITE) return false;
	//ZZ_LOG("vfs_pkg:open(%s)\n", filename);
	//ZZ_PROFILER_INSTALL(vfs_pkg_open);
	zz_assert(filename_in);
	zz_assert(filename_in[0]);

	zz_slash_converter filename(filename_in);

	if (fp_) {
		close();
	}

	assert(!fp_);
	if (!pkg_system_) {
		ZZ_LOG("vfs_pkg: open(%s) failed. invalid pkg_system_.\n", filename.get());
		return false;
	}

	//ZZ_LOG("vfs_pkg: open(%s)\n", filename);
	VHANDLE fsystem = pkg_system_->get_filesystem();

//	switch (mode) {
//		case zz_vfs::ZZ_VFS_READ:
			fp_ = VOpenFile(filename, fsystem);

			zz_assertf( fp_, "vfs_pkg: open(%s) failed.", filename.get() );

			filename_.set(filename);
			//check if it is encrypted
			fp_->btEncrypted=0;
			//if(fp_->btFileType==0){
			size_t spt;
			spt = fp_->sFileName.find_last_of("/\\");
			std::string hashstring1 = fp_->sFileName.substr(spt+1, (fp_->sFileName.length()-spt));
			//OutputDebugString(hashstring1.c_str());
			unsigned long key = StrToHashKey(hashstring1.c_str());
			char crypttable[16];
			DWORD *EAX=reinterpret_cast<DWORD*>(crypttable);
			DWORD EDI=0;
			DWORD ECX = key;
			DWORD EDX = key+1;
			ECX = ECX*4+1;
			for(int i = 0; i < 4; i++)
			{
				EDI=ECX*EDX;
				*((DWORD*)EAX) = EDI;
				EAX++;
				ECX+=4;
				EDX++;
			}
			vfseek(fp_, 0, VFSEEK_END);
			long off_set = vftell(fp_);
			if(off_set>16)
			{
				off_set-=16;
				vfseek(fp_,off_set, VFSEEK_SET);
				char buffer[16];
				vfread(&buffer,1,16,fp_);
				if(buffer[0]==crypttable[0]){
					if(buffer[1]==crypttable[1]){
						if(buffer[2]==crypttable[2]){
							if(buffer[3]==crypttable[3]){
								if(buffer[4]==crypttable[4]){
									if(buffer[5]==crypttable[5]){
										if(buffer[6]==crypttable[6]){
											if(buffer[7]==crypttable[7]){
												if(buffer[8]==crypttable[8]){
													if(buffer[9]==crypttable[9]){
														if(buffer[10]==crypttable[10]){
															if(buffer[11]==crypttable[11]){
																if(buffer[12]==crypttable[12]){
																	if(buffer[13]==crypttable[13]){
																		if(buffer[14]==crypttable[14]){
																			if(buffer[15]==crypttable[15]){
																				fp_->btEncrypted=0x90;
																				fp_->lEndOff-=16;
																				//MessageBox(0,fp_->sFileName.c_str()," is encrypted",0);
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			vfseek(fp_,0, VFSEEK_SET);
//			break;
//		case zz_vfs::ZZ_VFS_WRITE:
			// not implemented yet!!!
//			break;
//	}
	
	if (!fp_) {
		return false;
	}
	return true;
}

bool zz_vfs_pkg::close (void)
{
	//ZZ_LOG("vfs_pkg:close(%s)\n", filename_.get());
	//ZZ_PROFILER_INSTALL(vfs_pkg_close);

	set_status(zz_vfs::ZZ_VFS_INI);
	filename_.reset();
	if (fp_) {
		VCloseFile(fp_);
		fp_ = NULL;
	}
	return true;
}


uint32 zz_vfs_pkg::read_ (char * buf, const uint32 size)
{
	assert(size < MAX_FILESIZE);
	uint32 read_size = 0;
	if (fp_) {
		#define KEYSIZE 0x1000
		long Mpointer = vftell(fp_) % KEYSIZE;
		read_size = vfread(buf, 1, size, fp_);
		if(fp_->btEncrypted==0x90)
		{
			size_t spt;
			spt = fp_->sFileName.find_last_of("/\\");
			std::string hashstring1 = fp_->sFileName.substr(spt+1, (fp_->sFileName.length()-spt));
			unsigned long key = StrToHashKey(hashstring1.c_str());
			char crypttable[KEYSIZE];
			DWORD *EAX=reinterpret_cast<DWORD*>(crypttable);
			DWORD EDI=0;
			DWORD ECX = key;
			DWORD EDX = key+1;
			ECX = ECX*4+1;
			for(int i = 0; i < 0x400; i++)
			{
				EDI=ECX*EDX;
				*((DWORD*)EAX) = EDI;
				EAX++;
				ECX+=4;
				EDX++;
			}

			for(size_t k=0;k<read_size;k++)
			{
				*((char*)buf+k)^= crypttable[Mpointer];
				Mpointer++; Mpointer %= KEYSIZE;
			}
		}
	}
	else {
		ZZ_LOG("vfs_pkg: read_() failed. invalid fp_.\n");
		return 0;
	}

	assert(read_size < MAX_FILESIZE);
	if (read_size >= MAX_FILESIZE) {
		ZZ_LOG("vfs_pkg: read_(%s) failed. read_size = %d.\n",
			filename_.get(), read_size);
		read_size = 0;
	}
	return read_size;
}

uint32 zz_vfs_pkg::write_ (const char * buf, uint32 size)
{
	return 0;
}

bool zz_vfs_pkg::exist (const char * filename_in) const
{
	//ZZ_LOG("vfs_pkg: exist:\n");
	bool ret = false;

	zz_slash_converter filename(filename_in);
	if (!pkg_system_) {
		ZZ_LOG("vfs_pkg: exist(%s) failed. invalid pkg_system_.\n", filename);
		ret = false;
	}

	VHANDLE fsystem = pkg_system_->get_filesystem();
	assert(fsystem);
	
	ret = VFileExists(fsystem, filename);
	
	return ret;
}

uint32 zz_vfs_pkg::get_size () const
{
	//ZZ_LOG("vfs_pkg: get_size:\n");
	uint32 read_size = 0;

	if (!fp_) {
		ZZ_LOG("vfs_pkg: get_size(%s) failed. invalid fp_.\n", filename_.get());
		return 0;
	}

	assert(fp_);
	read_size = vfgetsize(fp_);
	
	assert(read_size < MAX_FILESIZE);
	if (read_size >= MAX_FILESIZE) {
		ZZ_LOG("vfs_pkg: get_size(%s) failed. read_size = %d.\n",
			filename_.get(), read_size);
		read_size = 0;
	}

	return uint32(read_size);
}

uint32 zz_vfs_pkg::get_size (const char * filename_in) const
{
	//ZZ_LOG("vfs_pkg: get_size(%s):\n", filename);
	uint32 read_size;
	zz_slash_converter filename(filename_in);

	assert(pkg_system_);
	if (!pkg_system_) {
		ZZ_LOG("vfs_pkg: get_size(%s) failed. invalid pkg_system_.\n", filename);
		return 0;
	}

	VHANDLE fsystem = pkg_system_->get_filesystem();
	
	read_size = VGetFileLength(fsystem, filename);
	
	assert(read_size < MAX_FILESIZE);
	if (read_size >= MAX_FILESIZE) {
		ZZ_LOG("vfs_pkg: get_size(%s) failed. read_size = %d.\n",
			filename, read_size);
		read_size = 0;
	}

	//ZZ_LOG("vfs_pkg: get_size() lock_count_(%d)\n", lock_count_);

	return read_size;
}

#endif // ZZ_USE_TRIGGERVFS