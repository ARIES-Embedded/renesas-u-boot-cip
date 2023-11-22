#
# board/aries/mrzg2ls/Makefile
#
# Copyright (C) 2023 ARIES Embedded
#
# SPDX-License-Identifier: GPL-2.0+
#

ifdef CONFIG_SPL_BUILD
obj-y	:= ../../renesas/rcar-common/gen3-spl.o
else
obj-y	:= mrzg2ls.o ../../renesas/rcar-common/common.o ../../renesas/rzf-dev/rzf-dev_spi.o
endif
