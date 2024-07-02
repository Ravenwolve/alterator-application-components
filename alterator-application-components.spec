%define _unpackaged_files_terminate_build 1

Name: alterator-application-components
Version: 0.1.0
Release: alt1

Summary: Alterator application for managing system components
License: GPLv2+
Group: System/Configuration/Other
URL: https://gitlab.basealt.space/alt/alterator-application-components

Source: %name-%version.tar

BuildRequires: cmake
BuildRequires: rpm-macros-cmake
BuildRequires: cmake-modules
BuildRequires: gcc-c++
BuildRequires: qt5-base-common qt5-base-devel qt5-declarative-devel qt5-tools-devel
BuildRequires: boost-devel-headers
BuildRequires: libqbase-devel

Requires: alterator-backend-packages alterator-entry libqbase alterator-interface-component alterator-backend-component_categories

%description
Alterator application for managing system components.

%prep
%setup

%build
%cmake
%cmake_build

%install
%cmakeinstall_std
mkdir -p %buildroot%_datadir/alterator/applications
mkdir -p %buildroot%_datadir/alterator/objects
mkdir -p %buildroot%_datadir/alterator/backends

install -v -p -m 644 -D alterator/components.object %buildroot%_datadir/alterator/objects
install -v -p -m 644 -D alterator/components-app.application %buildroot%_datadir/alterator/applications
install -v -p -m 644 -D alterator/components.backend %buildroot%_datadir/alterator/backends

%files
%_datadir/alterator/applications/*.application
%_datadir/alterator/backends/*.backend
%_datadir/alterator/objects/*.object
%_bindir/%name

%changelog
* Thu Mar 21 2024 Michael Chernigin <chernigin@altlinux.org> 0.1.0-alt1
- Initial build.
