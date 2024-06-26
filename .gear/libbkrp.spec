%define _unpackaged_files_terminate_build 1
%set_verify_elf_method unresolved=relaxed

Name: libbkrp
Version: 0.0.1
Release: alt1

Summary: Client library for MS-BKRP
License: GPLv2+
Group: Development
Url: https://github.com/august-alt/libbkrp

BuildRequires: rpm-macros-cmake cmake cmake-modules gcc-c++
BuildRequires: samba-devel samba-common-libs samba-pidl libkrb5-devel
BuildRequires: doxygen

Requires: cmake

Source0: %name-%version.tar

%description
Client library for MS-BKRP

%package devel
Summary: MS-BKRP Libraries and Header Files
Group: Development/C
Requires: %name = %version-%release

%description devel
%name-devel contains the libraries and header files needed to
develop programs which make use of %name

%prep
%setup -q

%build
%cmake
%cmake_build

%install
%cmakeinstall_std

%files
%_libdir/libbkrpclient.so.*

%files devel
%doc README.md
%doc INSTALL.md

%_includedir/bkrp/*.acf
%_includedir/bkrp/*.idl
%_includedir/bkrp/*.h

%_libdir/libbkrpclient.so

%_libdir/bkrp/BkrpConfig.cmake

%changelog
* Sun Apr 21 2024 Vladimir Rubanov <august@altlinux.org> 0.0.1-alt1
- 0.0.1-alt1
- Initial build
