# --------------------------------------------------------
# _clear version 2014.09.20
# rem Copyright (c) Gilbert Lee All rights reserved
# --------------------------------------------------------

# --------------------------------------------------------
# rm_file
# --------------------------------------------------------
rm_file() {
  echo "rm_file " "$1"
  find -name "$1" -exec rm -rf {} \;
}

# --------------------------------------------------------
# rm_folder
# --------------------------------------------------------
rm_folder() {
  echo "rm_folder " "$1"
  find -name "$1" -exec rm -rf {} \;
}

# --------------------------------------------------------
# common
# --------------------------------------------------------
rm_file *.obj
rm_file *.user
rm_file *.ini
rm_file *.bak
rm_file *.log
rm_file *.xml
rm_file *.json

# --------------------------------------------------------
# borland
# --------------------------------------------------------
rm_file *.~*
rm_file *.dcu
rm_file *.dsk
rm_file *.cfg
rm_file *.dsm
rm_file *.dcp
rm_file *.tds
rm_file *.#*
rm_file *.local
rm_file *.cgl
rm_file *.ilc
rm_file *.ild
rm_file *.ilf
rm_file *.ils
rm_file *.map
rm_file *.identcache

# --------------------------------------------------------
# microsoft
# --------------------------------------------------------
rm_file *.pch
rm_file *.idb
rm_file *.pdb
rm_file *.exp
rm_file *.ilk
rm_file *.ncb
rm_file *.aps
rm_file *.plg
rm_file *.opt
rm_file *.bsc
rm_file *.sbr
rm_file *.dep
rm_file *.manifest
rm_file *.sdf
rm_file *.manifest.res
rm_file *.sln.cache
rm_file BuildLog.htm
rm_file *.suo

# --------------------------------------------------------
# gcc
# --------------------------------------------------------
rm_file *.o

# --------------------------------------------------------
# qt
# --------------------------------------------------------
rm_file Makefile
rm_file Makefile.*
rm_file ui_*.h
rm_file qrc_*.cpp
rm_file moc_*.cpp
rm_folder GeneratedFiles

# --------------------------------------------------------
# other
# --------------------------------------------------------
# rm_file *.lib
# rm_file *.a
# rm_file *.exe

# --------------------------------------------------------
# folder
# --------------------------------------------------------
rm_folder Debug
rm_folder debug
rm_folder Release
rm_folder release
# rm_folder log
rm_folder Win32
rm_folder x64
rm_folder cache
rm_folder __history

# --------------------------------------------------------
# epilogue
# --------------------------------------------------------
# goto :eof
