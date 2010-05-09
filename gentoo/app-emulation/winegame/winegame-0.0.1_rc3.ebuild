# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="3"


inherit qt4-r2

DESCRIPTION="WineGame is a small Qt4 application, allows you to install Windows games in Wine"
HOMEPAGE="http://winegame.googlecode.com"
SRC_URI="http://winegame.googlecode.com/files/${P}.tar.gz"

LICENSE="GPL-3"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

DEPEND="=dev-libs/libwinegame-${PV}
	 sys-fs/fuseiso"

PATCHES=( 
"${FILESDIR}/fixpro.patch"
)
src_install () {
# Do default install
  qt4-r2_src_install
# Install docs
  dodoc README CHANGELOG TODO VERSION LICENSE
}