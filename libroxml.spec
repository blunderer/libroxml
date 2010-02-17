%define name libroxml
%define version 2.0
%define release 1

Summary: a light and powerfull xml parsing library
Name: %{name}
Version: %{version}
Release: %{release}
Source: http://libroxml.googlecode.com/files/libroxml-%{version}.tar.gz
Vendor: blunderer
URL:  http://code.google.com/p/libroxml/
License: LGPL
Group: System Environment/Libraries
Prefix: %{_prefix}

%description
libroxml is a very light, low fingerprint, fast, and powerfull
library designed for parsing XML files. This is perfectly suitable
for little embeded systems but you also use it widely in various
application for xml configuration file reading for example.

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
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%doc debian/changelog debian/README
%{_prefix}/lib/lib*.so
%{_prefix}/lib/lib*.a

%files devel
%defattr(-,root,root)
%doc debian/changelog debian/README
%{_prefix}/bin/*
%{_prefix}/lib/lib*.so
%{_prefix}/include/*
%{_prefix}/lib/pkgconfig/*
%{_prefix}/share/doc/libroxml/html/*

