%define name libroxml
%define version 2.2.0
%define release 1

Summary: a light and powerful C xml parsing library
Name: %{name}
Version: %{version}
Release: %{release}
Source: http://libroxml.googlecode.com/files/libroxml-%{version}.tar.gz
Vendor: blunderer
URL:  http://www.libroxml.net
License: LGPL
Group: System Environment/Libraries
Prefix: %{_prefix}

%description
libroxml is a very light, low fingerprint, fast, and powerful
library designed for parsing XML files. This is perfectly suitable
for little embedded systems but you can also use it widely in various
applications for xml configuration file reading for example.

%package devel
Summary: development file for libroxml
Group: Development/Libraries
Requires: %{name} = %{version}

%description devel
The %{name}-devel package contains the header files and static libraries for
building applications which use %{name}.

%prep
%setup -n %{name}-%{version}

%build
make

%install
rm -rf ${RPM_BUILD_ROOT}
make DESTDIR=${RPM_BUILD_ROOT} install

%clean
rm -rf ${RPM_BUILD_ROOT}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%doc debian/changelog debian/README
%{_prefix}/lib/libroxml.so.0
%{_prefix}/bin/roxml
%{_prefix}/share/man/man1/roxml.1.gz

%files devel
%defattr(-,root,root)
%doc debian/changelog debian/README
%{_prefix}/lib/libroxml.a
%{_prefix}/lib/libroxml.so
%{_prefix}/include/roxml.h
%{_prefix}/lib/pkgconfig/libroxml.pc
%{_prefix}/share/doc/libroxml/html/*
%{_prefix}/share/man/man3/*.3.gz
