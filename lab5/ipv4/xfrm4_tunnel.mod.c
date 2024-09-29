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
	{ 0xe6905823, "xfrm4_tunnel_deregister" },
	{ 0xe9bd3f25, "xfrm_unregister_type" },
	{ 0x62810141, "xfrm4_tunnel_register" },
	{ 0xc5850110, "printk" },
	{ 0x24dff578, "xfrm_register_type" },
	{ 0xf4271b01, "skb_push" },
	{ 0xf1e395a7, "xfrm_input" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=tunnel4";


MODULE_INFO(srcversion, "BE38A7F5AB1543CF3B8EFAF");
