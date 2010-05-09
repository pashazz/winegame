# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
inherit subversion
DESCRIPTION="Quick and dirty script to download and install various redistributable runtime libraries"
HOMEPAGE="http://wiki.winehq.org/winetricks"
ESVN_REPO_URI="http://winezeug.googlecode.com/svn/trunk/"
SRC_URI=""

LICENSE="LGPL"
SLOT="0"
KEYWORDS="~x86 ~amd64"
IUSE=""

DEPEND="app-emulation/wine
	app-arch/cabextract"
RDEPEND="${DEPEND}"

src_install()
{
	dobin winetricks
}

