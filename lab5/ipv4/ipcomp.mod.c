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
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
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
	{ 0x1e190136, "ipcomp_output" },
	{ 0x831ae46d, "ipcomp_input" },
	{ 0x8f76d2e8, "ipcomp_destroy" },
	{ 0xf1e395a7, "xfrm_input" },
	{ 0xf274c24d, "xfrm4_rcv" },
	{ 0xf1f00d08, "xfrm4_protocol_deregister" },
	{ 0xe9bd3f25, "xfrm_unregister_type" },
	{ 0x2789f3, "xfrm4_protocol_register" },
	{ 0xc5850110, "printk" },
	{ 0x24dff578, "xfrm_register_type" },
	{ 0x83e58dc6, "ipv4_update_pmtu" },
	{ 0xa99d147f, "ipv4_redirect" },
	{ 0xf647b45c, "__xfrm_state_destroy" },
	{ 0x55ce2c1f, "xfrm_state_insert" },
	{ 0x69ba0a7a, "xfrm_init_state" },
	{ 0xd2b3df35, "xfrm_state_alloc" },
	{ 0x7bc9a3d1, "xfrm_state_lookup" },
	{ 0x165b145c, "ex_handler_refcount" },
	{ 0xddc437fa, "ipcomp_init_state" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=xfrm_ipcomp";


MODULE_INFO(srcversion, "69B32AE1DE5027AEE5F0E43");
