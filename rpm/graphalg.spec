Name:           graphalg
Version:        0.1.0
Release:        1%{?dist}
Summary:        Graph algorithms on the graphData public API

License:        MIT
URL:            https://github.com/david-days/graphAlg
# GitHub archive; #/ sets the SOURCES basename for rpmbuild/spectool.
Source0:        https://github.com/david-days/graphAlg/archive/refs/tags/v%{version}.tar.gz#/graphAlg-%{version}.tar.gz

BuildRequires:  cmake >= 3.25
BuildRequires:  gcc
BuildRequires:  ninja-build
BuildRequires:  pkgconfig(graphdata)
BuildRequires:  graphdata-devel

Requires:       graphdata%{?_isa}

%description
Native C library of graph algorithms that depend only on the installed
graphData public API (paths, trees, connected components, and related search).

%package devel
Summary:        Development files for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}
Requires:       graphdata-devel%{?_isa}

%description devel
Headers and pkg-config file for building against %{name}.

%prep
# GitHub archive directory uses the repo name casing: graphAlg-VERSION
%autosetup -n graphAlg-%{version}

%build
# CMake root is the repository root. Skip tests (no Check in buildroot).
%cmake -DBUILD_TESTING=OFF
%cmake_build --target graphAlg

%install
%cmake_install

# First COPR pass: no %%check. Add check-devel + ctest once the package builds.

%files
%license LICENSE
%doc README.md
%{_libdir}/libgraphAlg.so.*

%files devel
%{_includedir}/graphAlg.h
%{_includedir}/graphAlgCut.h
%{_includedir}/graphAlgSearch.h
%{_libdir}/libgraphAlg.so
%{_libdir}/pkgconfig/graphalg.pc

%changelog
* Sat Nov 14 2026 David C Days <david.c.days@gmail.com> - 0.1.0-1
- Initial COPR / Fedora-style package
