import os
import urllib.request
from zipfile import ZipFile
import tarfile
import shutil
import subprocess

# http://stackoverflow.com/a/377028/2606891
def which(program):
    import os
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None

	
def extract_zip(what, where):
	print("extracting {}...".format(what))
	zip_file = ZipFile(what)
	zip_file.extractall(path=where)
	zip_file.close()
	

def extract_tar(what, where):
	print("extracting {}...".format(what))
	tar_file = tarfile.open(what)
	tar_file.extractall(path=where)
	tar_file.close()

	
def download(url, path):
	urllib.request.urlretrieve(url, path)
	

def patch(path, source, target):
	print("PATCHING {}".format(path))
	fin = open(path, "r")
	text = fin.read()
	fin.close()
	# some BOM autism
	if ord(text[0]) == 239:
		text = text[3:]
	text = text.replace(source, target)
	fout = open(path, "w", encoding="utf-8")
	fout.write(text)
	fout.close()

	
LIBOGG_VERSION = "1.3.1"
irrlicht = "irrlicht-1.8.1"
curl = "curl-7.34.0"
openal = "openal-soft-1.15.1"
libogg = "libogg-{}".format(LIBOGG_VERSION)
libvorbis = "libvorbis-1.3.3"
zlib = "zlib-1.2.8"
freetype = "freetype-2.5.2"
luajit = "LuaJIT-2.0.2"
gettext = "gettext-0.13.1"
libiconv = "libiconv-1.9.1"

def main():
	if not os.path.exists("settings.py"):
		print("Please, run build.py from build/windows directory and make sure you copied settings.py.example to settings.py and configured it.")
		return
	
	from settings import LEVELDB_PATH, BOOST_PATH

	msbuild = which("MSBuild.exe")
	cmake = which("cmake.exe")
	if not msbuild:
		print("MSBuild.exe not found! Make sure you run 'Visual Studio Command Prompt', not cmd.exe")
		return
	if not cmake:
		print("cmake.exe not found! Make sure you have CMake installed and added to PATH.")
	print("Found msbuild: {}\nFound cmake: {}".format(msbuild, cmake))
	
	if not os.path.exists("deps"):
		print("Creating `deps` directory.")
		os.mkdir("deps")

	os.chdir("deps")
	if not os.path.exists(irrlicht):
		print("Irrlicht not found, downloading.")
		zip_path = "{}.zip".format(irrlicht)
		urllib.request.urlretrieve("http://downloads.sourceforge.net/irrlicht/{}.zip".format(irrlicht), zip_path)
		extract_zip(zip_path, ".")
		os.chdir(os.path.join(irrlicht, "source", "Irrlicht"))
		# sorry but this breaks the build
		patch(os.path.join("zlib", "deflate.c"), "const char deflate_copyright[] =", "static const char deflate_copyright[] =")
		os.system('MSBuild Irrlicht10.0.sln /p:Configuration="Static lib - Release"')
		os.chdir(os.path.join("..", "..", ".."))

	if not os.path.exists(curl):
		print("curl not found, downloading.")
		os.mkdir(curl)
		tar_path = "{}.tar.gz".format(curl)
		urllib.request.urlretrieve("http://curl.haxx.se/download/{}.tar.gz".format(curl), tar_path)
		extract_tar(tar_path, ".")
		os.chdir(os.path.join(curl, "winbuild"))
		os.system("nmake /f Makefile.vc mode=static RTLIBCFG=static USE_IDN=no")
		os.chdir(os.path.join("..", ".."))
	
	if not os.path.exists(openal):
		print("openal not found, downloading.")
		tar_path = "{}.tar.bz2".format(openal)
		download("http://kcat.strangesoft.net/openal-releases/{}.tar.bz2".format(openal), tar_path)
		extract_tar(tar_path, ".")
		print("building openal")
		os.chdir(os.path.join(openal, "build"))
		os.system("cmake .. -DFORCE_STATIC_VCRT=1 -DLIBTYPE=STATIC")
		os.system("MSBuild ALL_BUILD.vcxproj /p:Configuration=Release")
		os.chdir(os.path.join("..", ".."))
	
	if not os.path.exists(libogg):
		print("libogg not found, downloading.")
		tar_path = "{}.tar.gz".format(libogg)
		download("http://downloads.xiph.org/releases/ogg/{}.tar.gz".format(libogg), tar_path)
		extract_tar(tar_path, ".")
		print("building libogg")
		os.chdir(os.path.join(libogg, "win32", "VS2010"))
		os.system("MSBuild libogg_static.vcxproj /p:Configuration=Release")
		os.chdir(os.path.join("..", "..", ".."))
	
	if not os.path.exists(libvorbis):
		print("libvorbis not found, downloading.")
		tar_path = "{}.tar.gz".format(libvorbis)
		download("http://downloads.xiph.org/releases/vorbis/{}.tar.gz".format(libvorbis), tar_path)
		extract_tar(tar_path, ".")
		print("building libvorbis")
		os.chdir(os.path.join(libvorbis, "win32", "VS2010"))
		# patch libogg.props to have correct version of libogg
		patch("libogg.props", "<LIBOGG_VERSION>1.2.0</LIBOGG_VERSION>", "<LIBOGG_VERSION>{}</LIBOGG_VERSION>".format(LIBOGG_VERSION))
		# not static enough!
		patch(os.path.join("libvorbisfile", "libvorbisfile_static.vcxproj"), "<RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>", "<RuntimeLibrary>MultiThreaded</RuntimeLibrary>")
		patch(os.path.join("libvorbis", "libvorbis_static.vcxproj"), "<RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>", "<RuntimeLibrary>MultiThreaded</RuntimeLibrary>")

		os.system("MSBuild vorbis_static.sln /p:Configuration=Release")
		os.chdir(os.path.join("..", "..", ".."))
		
	if not os.path.exists(zlib):
		print("zlib not found, downloading.")
		tar_path = "{}.tar.gz".format(zlib)
		download("http://prdownloads.sourceforge.net/libpng/{}.tar.gz?download".format(zlib), tar_path)
		extract_tar(tar_path, ".")
		print("building zlib")
		os.chdir(os.path.join(zlib, "contrib", "vstudio", "vc10"))
		os.system("MSBuild zlibvc.sln /p:Configuration=Release /p:Platform=win32")
		os.chdir(os.path.join("..", "..", "..", ".."))
	
	if not os.path.exists(freetype):
		print("freetype not found, downloading.")
		tar_path = "{}.tar.gz".format(freetype)
		download("http://download.savannah.gnu.org/releases/freetype/{}.tar.gz".format(freetype), tar_path)
		extract_tar(tar_path, ".")
		print("building freetype")
		os.chdir(os.path.join(freetype, "builds", "windows", "vc2010"))
		os.system('MSBuild freetype.vcxproj /p:Configuration="Release Multithreaded"')
		os.chdir(os.path.join("..", "..", "..", ".."))
	
	if not os.path.exists(luajit):
		print("LuaJIT not found, downloading.")
		tar_path = "{}.tar.gz".format(luajit)
		download("http://luajit.org/download/{}.tar.gz".format(luajit), tar_path)
		extract_tar(tar_path, ".")
		print("building LuaJIT")
		os.chdir(os.path.join(luajit, "src"))
		patch("msvcbuild.bat", "/MD", "/MT")
		os.system("msvcbuild.bat static")
		os.chdir(os.path.join("..", ".."))
		
	if not os.path.exists(gettext):
		print("gettext/libiconv not found, downloading")
		tar_path = "{}.tar.gz".format(gettext)
		download("http://ftp.gnu.org/gnu/gettext/{}.tar.gz".format(gettext), tar_path)
		extract_tar(tar_path, ".")
		tar_path = "{}.tar.gz".format(libiconv)
		download("http://ftp.gnu.org/gnu/libiconv/{}.tar.gz".format(libiconv), tar_path)
		extract_tar(tar_path, ".")
		
		os.chdir(libiconv)
		os.system("nmake -f Makefile.msvc NO_NLS=1 MFLAGS=-MT")
		os.system("nmake -f Makefile.msvc NO_NLS=1 MFLAGS=-MT install")
		os.system("nmake -f Makefile.msvc NO_NLS=1 MFLAGS=-MT distclean")
		os.chdir("..")
		os.chdir(gettext)
		patch(os.path.join("gettext-runtime", "intl", "localename.c"), "case SUBLANG_PUNJABI_PAKISTAN:", "//case SUBLANG_PUNJABI_PAKISTAN:")
		patch(os.path.join("gettext-runtime", "intl", "localename.c"), "case SUBLANG_ROMANIAN_MOLDOVA:", "//case SUBLANG_ROMANIAN_MOLDOVA:")
		os.system("nmake -f Makefile.msvc MFLAGS=-MT")
		os.system("nmake -f Makefile.msvc MFLAGS=-MT install")
		os.chdir("..")
		os.chdir(libiconv)
		os.system("nmake -f Makefile.msvc MFLAGS=-MT")
		# DARK MSVC MAGIC
		os.environ["CL"] = "/Za"
		os.system("nmake -f Makefile.msvc MFLAGS=-MT")
		os.environ["CL"] = ""
		os.system("nmake -f Makefile.msvc MFLAGS=-MT")
		# /magic
		os.system("nmake -f Makefile.msvc MFLAGS=-MT install")
		
		os.chdir("..")
		
	
	os.chdir("..")
	
	print("=> Building Freeminer")
	# multi-process build
	os.environ["CL"] = "/MP"
	if os.path.exists("build_tmp"):
		shutil.rmtree("build_tmp")
	if os.path.exists("install_tmp"):
		shutil.rmtree("install_tmp")
	os.mkdir("build_tmp")
	os.chdir("build_tmp")
	cmake_string = r"""
		-DCMAKE_BUILD_TYPE=Release
		-DRUN_IN_PLACE=1
		-DCUSTOM_BINDIR=.
		-DCMAKE_INSTALL_PREFIX=..\install_tmp\
		-DSTATIC_BUILD=1
		-DIRRLICHT_SOURCE_DIR=..\deps\{irrlicht}\
		-DENABLE_SOUND=1
		-DENABLE_MANDELBULBER=1
		-DOPENAL_INCLUDE_DIR=..\deps\{openal}\include\AL\
		-DOPENAL_LIBRARY=..\deps\{openal}\build\Release\OpenAL32.lib
		-DOGG_INCLUDE_DIR=..\deps\{libogg}\include\
		-DOGG_LIBRARY=..\deps\{libogg}\win32\VS2010\Win32\Release\libogg_static.lib
		-DVORBIS_INCLUDE_DIR=..\deps\{libvorbis}\include\
		-DVORBIS_LIBRARY=..\deps\{libvorbis}\win32\VS2010\Win32\Release\libvorbis_static.lib
		-DVORBISFILE_LIBRARY=..\deps\{libvorbis}\win32\VS2010\Win32\Release\libvorbisfile_static.lib
		-DZLIB_INCLUDE_DIR=..\deps\{zlib}\
		-DZLIB_LIBRARIES=..\deps\{zlib}\contrib\vstudio\vc10\x86\ZlibStatRelease\zlibstat.lib
		-DENABLE_FREETYPE=1
		-DFREETYPE_INCLUDE_DIR_freetype2=..\deps\{freetype}\include\
		-DFREETYPE_INCLUDE_DIR_ft2build=..\deps\{freetype}\include\
		-DFREETYPE_LIBRARY=..\deps\{freetype}\objs\win32\vc2010\freetype252MT.lib
		-DLUA_LIBRARY=..\deps\{luajit}\src\lua51.lib
		-DLUA_INCLUDE_DIR=..\deps\{luajit}\src\
		-DENABLE_CURL=1
		-DCURL_LIBRARY=..\deps\{curl}\builds\libcurl-vc-x86-release-static-ipv6-sspi-spnego-winssl\lib\libcurl_a.lib
		-DCURL_INCLUDE_DIR=..\deps\{curl}\builds\libcurl-vc-x86-release-static-ipv6-sspi-spnego-winssl\include
		-DGETTEXT_INCLUDE_DIR=C:\usr\include\
		-DGETTEXT_LIBRARY=C:\usr\lib\intl.lib
		-DICONV_LIBRARY=C:\usr\lib\iconv.lib
		-DGETTEXT_MSGFMT=C:\usr\bin\msgfmt.exe
		-DENABLE_GETTEXT=1
		-DLEVELDB_INCLUDE_DIR={leveldb}\include\
		-DLEVELDB_LIBRARY={leveldb}\Release\leveldb.lib
		-DENABLE_LEVELDB=1
	""".format(irrlicht=irrlicht, zlib=zlib, freetype=freetype, luajit=luajit, openal=openal, libogg=libogg, libvorbis=libvorbis, curl=curl, leveldb=LEVELDB_PATH).replace("\n", "")
	
	os.system(r"cmake ..\..\.. " + cmake_string)
	patch(os.path.join("src", "freeminer.vcxproj"), "</AdditionalLibraryDirectories>", r";$(DXSDK_DIR)\Lib\x86;{boost}\lib32-msvc-10.0</AdditionalLibraryDirectories>".format(boost=BOOST_PATH))
	os.system("MSBuild ALL_BUILD.vcxproj /p:Configuration=Release")
	os.system("MSBuild INSTALL.vcxproj /p:Configuration=Release")
	os.system("MSBuild PACKAGE.vcxproj /p:Configuration=Release")
	
	
	
if __name__ == "__main__":
	main()
