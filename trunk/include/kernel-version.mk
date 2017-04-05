# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.14 = .38
LINUX_VERSION-3.18 = .21
LINUX_VERSION-4.0 = .9
LINUX_VERSION-4.1 = .6
LINUX_VERSION-3.4 = .69

LINUX_KERNEL_MD5SUM-3.4.69  = ce1738a31df15a58a4b90cde0bd8fb6b
LINUX_KERNEL_MD5SUM-3.14.38 = a62c35b24b3c61b35f20a1e0b837483f
LINUX_KERNEL_MD5SUM-3.18.21 = e4248caaa4cef318c04657e971b37298
LINUX_KERNEL_MD5SUM-4.0.9 = 40fc5f6e2d718e539b45e6601c71985b
LINUX_KERNEL_MD5SUM-4.1.6 = 1dae0c808e34164cab3dfd57be88bd53

ifdef KERNEL_PATCHVER
  LINUX_VERSION:=$(KERNEL_PATCHVER)$(strip $(LINUX_VERSION-$(KERNEL_PATCHVER)))
endif

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
KERNEL_PATCHVER ?= $(KERNEL)

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM:=$(LINUX_KERNEL_MD5SUM-$(strip $(LINUX_VERSION)))
LINUX_KERNEL_MD5SUM?=x
