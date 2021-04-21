all: release

DEVICE_IP ?= "10.11.99.1"
# DEVICE_IP ?= "192.168.0.34"

SSH_DIR ?= $(HOME)/.ssh
# SSH_DIR ?= $(shell cygpath -w ~/.ssh)
# SSH_DIR ?= G:\cygwin64\home\Eeems\.ssh

QTCREATOR_RUN_ARGS ?= --gpus all
# QTCREATOR_RUN_ARGS ?=

ifeq ($(OS),Windows_NT)
	BUILD_DIR ?= $(shell cygpath -w `pwd`)
	DISPLAY ?= host.docker.internal:0.0
	DISPLAY_ARGS ?=
else
	BUILD_DIR ?= $(shell pwd)
	DISPLAY ?= unix$(DISPLAY)
	DISPLAY_ARGS ?= -v /tmp/.X11-unix:/tmp/.X11-unix --device /dev/snd
endif


.PHONY: docker-qtcreator
docker-qtcreator:
	cd docker-toolchain/qtcreator && docker build \
		--tag rm-qtcreator .

qtcreator: docker-qtcreator
	xhost +SI:localuser:root > /dev/null
	docker start -a qtcreator || \
	docker run \
		$(QTCREATOR_RUN_ARGS) \
		--name qtcreator \
		-v '$(BUILD_DIR):/root/project:rw' \
		-v '$(SSH_DIR):/root/.ssh' \
		-v '$(BUILD_DIR)/docker-toolchain/qtcreator/files/config:/root/.config' \
		-w /root/project \
		-e DISPLAY=$(DISPLAY) \
		$(DISPLAY_ARGS) \
		rm-qtcreator:latest

clean:
	rm -rf .build
	rm -rf release

release: clean
	$(MAKE) build
	mkdir -p release
	INSTALL_ROOT=../../release $(MAKE) -C .build/process-manager install
	INSTALL_ROOT=../../release $(MAKE) -C .build/system-service install
	INSTALL_ROOT=../../release $(MAKE) -C .build/settings-manager install
	INSTALL_ROOT=../../release $(MAKE) -C .build/screenshot-tool install
	INSTALL_ROOT=../../release $(MAKE) -C .build/screenshot-viewer install
	INSTALL_ROOT=../../release $(MAKE) -C .build/launcher install
	INSTALL_ROOT=../../release $(MAKE) -C .build/lockscreen install
	INSTALL_ROOT=../../release $(MAKE) -C .build/task-switcher install
	INSTALL_ROOT=../../release $(MAKE) -C .build/web-browser install

build: tarnish erode rot oxide decay corrupt fret anxiety mold

erode:
	mkdir -p .build/process-manager
	cp -r applications/process-manager/* .build/process-manager
	cd .build/process-manager && qmake erode.pro
	$(MAKE) -C .build/process-manager all

tarnish:
	mkdir -p .build/system-service
	cp -r applications/system-service/* .build/system-service
	cd .build/system-service && qmake tarnish.pro
	$(MAKE) -C .build/system-service all

rot: tarnish
	mkdir -p .build/settings-manager
	cp -r applications/settings-manager/* .build/settings-manager
	cd .build/settings-manager && qmake rot.pro
	$(MAKE) -C .build/settings-manager all

fret: tarnish
	mkdir -p .build/screenshot-tool
	cp -r applications/screenshot-tool/* .build/screenshot-tool
	cd .build/screenshot-tool && qmake fret.pro
	$(MAKE) -C .build/screenshot-tool all

oxide: tarnish
	mkdir -p .build/launcher
	cp -r applications/launcher/* .build/launcher
	cd .build/launcher && qmake oxide.pro
	$(MAKE) -C .build/launcher all

decay: tarnish
	mkdir -p .build/lockscreen
	cp -r applications/lockscreen/* .build/lockscreen
	cd .build/lockscreen && qmake decay.pro
	$(MAKE) -C .build/lockscreen all

corrupt: tarnish
	mkdir -p .build/task-switcher
	cp -r applications/task-switcher/* .build/task-switcher
	cd .build/task-switcher && qmake corrupt.pro
	$(MAKE) -C .build/task-switcher all
	
anxiety: tarnish
	mkdir -p .build/screenshot-viewer
	cp -r applications/screenshot-viewer/* .build/screenshot-viewer
	cd .build/screenshot-viewer && qmake anxiety.pro
	$(MAKE) -C .build/screenshot-viewer all

mold: tarnish
	mkdir -p .build/web-browser
	cp -r applications/web-browser/* .build/web-browser
	cd .build/web-browser && qmake mold.pro
	$(MAKE) -C .build/web-browser all
