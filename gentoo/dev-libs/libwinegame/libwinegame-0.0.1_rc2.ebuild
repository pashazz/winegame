# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="3"


inherit qt4-r2

DESCRIPTION="WineGame library is a WineGame engine. It allows install windows apps into wine"
HOMEPAGE="http://winegame.googlecode.com"
SRC_URI="http://winegame.googlecode.com/files/${P}.tar.gz"

LICENSE="GPL-3"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

DEPEND=">=x11-libs/qt-gui-4.6.1
	>=x11-libs/qt-core-4.6.1
	app-emulation/wine"


src_install () {
# Do default install
  qt4-r2_src_install
# Install my headers (not installs by default because of binary distros
  mkdir -p "${D}"/usr/include/libwinegame
  cp "${S}"/*.h "${D}"/usr/include/libwinegame
}