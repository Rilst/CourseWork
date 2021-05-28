#
# Copyright (C) 2018-2020 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#
TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
           minigmp \
           src \
           qaesencryption \


minigmp.file = src/mini-gmp/GMP.pro
src.file = src/Qt-Secret.pro
qaesencryption.file = src/Qt-AES/qaesencryption.pro


