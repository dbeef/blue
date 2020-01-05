all: info

.PHONY: info
info:
	@echo "Install packages for OS. See file for the details."

.PHONY: fedora
fedora:
	sudo xargs -P1 -a packages.fedora dnf install -y

glm_fed30=http://download-ib01.fedoraproject.org/pub/fedora/linux/releases/30/Everything/x86_64/os/Packages/g/glm-devel-0.9.9.2-2.fc30.noarch.rpm
fedora29: fedora
	 wget $(glm_fed30)
	 sudo dnf --refresh install glm-devel-0.9.9.2-2.fc30.noarch.rpm
