all: info

.PHONY: info
info:
	@echo "Install packages for OS. See file for the details."

glm_fed30=http://download-ib01.fedoraproject.org/pub/fedora/linux/releases/30/Everything/x86_64/os/Packages/g/glm-devel-0.9.9.2-2.fc30.noarch.rpm
.PHONY: fedora
fedora:
	sudo xargs -P1 -a packages.fedora dnf install -y
	 wget $(glm_fed30)
	 sudo dnf --refresh install glm-devel-0.9.9.2-2.fc30.noarch.rpm

BUNDLE_DIR=bundle/
VCPKG_DIR=$(BUNDLE_DIR)/vcpkg
VCPKG=$(VCPKG_DIR)/vcpkg

$(VCPKG):
	-git clone https://github.com/Microsoft/vcpkg.git $(VCPKG_DIR)
	cd $(VCPKG_DIR) && ./bootstrap-vcpkg.sh
	cd $(VCPKG_DIR) && ./vcpkg integrate install

.PHONY: assimp
assimp: $(VCPKG)
	$(VCPKG) install assimp

.PHONY: SDL2
SDL2: $(VCPKG)
	$(VCPKG) install SDL2


BUILD_DIR=cmake-build-debug-ccache
.PHONY: build
build: | SDL2 assimp
	cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/local/bin/gcc -DCMAKE_CXX_COMPILER=/usr/local/bin/g++ -DCMAKE_TOOLCHAIN_FILE=bundle/vcpkg/scripts/buildsystems/vcpkg.cmake -G "CodeBlocks - Unix Makefiles"
	cmake --build $(BUILD_DIR) --target ClearColorSample -j 4
	cmake --build $(BUILD_DIR) -j 4

.PHONY: run
run:
	./$(BUILD_DIR)/examples/1-clear-color/ClearColorSample

IMAGE=blue-build
.PHONY: build-docker-image
build-docker-image:
	cd docker && sudo docker build . -t $(IMAGE)

.PHONY: build-docker
build-docker:
	sudo docker run -it --rm -v $(PWD):/project $(IMAGE) make -f INSTALL.mk build BUILD_DIR=cmake-build-docker