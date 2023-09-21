%define version 0.72b
%define release %mkrel 1

%define is_x86_64 %(if [ "x86_64" == `uname -m` ]; then echo "yes"; else echo "no"; fi)
%define use_xdg_menu %{!?_with_xdg_menu:0}%{?_with_xdg_menu:1}

Summary:  Schuman Resonance Oscillator
Name:     schuman
Version:  %{version}
Release:  %{release}
Source:   schuman-%{version}.tar.gz
License:  GPL
URL:      http://twitter.com/Almightroid/
Group:    Sound
Packager: Almightroid <unknown@anonymo.us>
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
Schuman Resonance Oscillator

%prep
%setup -q

%build
./configure

make clean
make

%install
rm -rf %{buildroot}
%makeinstall

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
%doc README
%{_bindir}/schuman

%changelog
* Thu Mar 5 2015 ScrapWare <unknown@anonymo.us>
- oops!
