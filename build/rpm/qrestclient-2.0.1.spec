#
# spec file for package qrestclient-2.0.1
#
# Copyright (c) 2015 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

Name:           qrestclient
Version:        2.0.1
Release:        1
License:        GPL
Summary:        Rest client
Url:            https://github.com/peterkomar/qrestclient
Vendor:         Peter Komar
Group:          Development/Networking
Source:         qrestclient-2.0.1.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
QRestClient testing tool for sending REST requests with different GET, POST, PUT, DELETE parameters, heders, raw body etc.

This tool will be helpful for developers working on web API services.

Featuers:

 * Execution time calculation
 * Parse GET params from URL
 * Requests history & cleaner
 * JSON response viewer
 * CSV response viewer
 * Request details viewer
 * Share request to Gist
%prep
%setup -q

%build
qmake-qt5
make %{?_smp_mflags}

%install
mkdir -p %{buildroot}/usr/bin
mkdir -p %{buildroot}/usr/share/applications
mkdir -p %{buildroot}/usr/share/icons
cp bin/qrestclient %{buildroot}/usr/bin
cp -R desktop/qrestclient.desktop %{buildroot}/usr/share/applications
cp -R -f desktop/icons %{buildroot}/usr/share
%clean
rm -rf $RPM_BUILD_ROOT
%post

%postun

%files
%defattr(-,root,root)
/usr/bin/qrestclient
/usr/share/applications/qrestclient.desktop
/usr/share/icons/hicolor/16x16/apps/qrestclient.png
/usr/share/icons/hicolor/24x24/apps/qrestclient.png
/usr/share/icons/hicolor/32x32/apps/qrestclient.png
/usr/share/icons/hicolor/48x48/apps/qrestclient.png
/usr/share/icons/hicolor/64x64/apps/qrestclient.png
/usr/share/icons/hicolor/128x128/apps/qrestclient.png
/usr/share/icons/hicolor/256x256/apps/qrestclient.png
/usr/share/icons/hicolor/512x512/apps/qrestclient.png
%changelog
* Thu Dec 2 2015 Peter Komar <peter0komar@gmail.com>
- Builded app for openSUSE Leap 42.1

