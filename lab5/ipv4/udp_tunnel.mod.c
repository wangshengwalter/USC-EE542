#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.arch = MODULE_ARCH_INIT,
};

MODULE_INFO(intree, "Y");

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xa29affe8, "module_layout" },
	{ 0x754d539c, "strlen" },
	{ 0xa4c149d4, "sock_release" },
	{ 0x184ffb5b, "metadata_dst_alloc" },
	{ 0xb58ad368, "sock_create_kern" },
	{ 0x493b230f, "kernel_setsockopt" },
	{ 0x1c19c7df, "kernel_connect" },
	{ 0xe703ca99, "iptunnel_xmit" },
	{ 0x47c107bc, "kernel_sock_shutdown" },
	{ 0x99517682, "udp_encap_enable" },
	{ 0x7da90eb3, "dev_get_by_index" },
	{ 0x24c7d57c, "ipv6_stub" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x2ea2c95c, "__x86_indirect_thunk_rax" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xbdda9168, "udp_set_csum" },
	{ 0xd952fbe5, "kernel_bind" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "B85B1D59FB8F1E83D9B5EEE");
