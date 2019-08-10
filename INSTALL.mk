all: info

.PHONY: info
info:
	@echo "Install packages for OS. See file for the details."

.PHONY: fedora
fedora:
	sudo xargs -P1 -a packages.fedora dnf install -y
