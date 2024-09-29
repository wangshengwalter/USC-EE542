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
	{ 0xf1e395a7, "xfrm_input" },
	{ 0xf274c24d, "xfrm4_rcv" },
	{ 0xf1f00d08, "xfrm4_protocol_deregister" },
	{ 0xe9bd3f25, "xfrm_unregister_type" },
	{ 0x2789f3, "xfrm4_protocol_register" },
	{ 0x24dff578, "xfrm_register_type" },
	{ 0xa3327a1, "xfrm_input_resume" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0x97651e6c, "vmemmap_base" },
	{ 0x4c9d28b0, "phys_base" },
	{ 0x86dff270, "__pskb_pull_tail" },
	{ 0x5a44f8cb, "__crypto_memneq" },
	{ 0x6fa692c6, "crypto_ahash_digest" },
	{ 0x6e29c488, "skb_to_sgvec_nomark" },
	{ 0xf888ca21, "sg_init_table" },
	{ 0xf4271b01, "skb_push" },
	{ 0xfb578fc5, "memset" },
	{ 0x122e64e0, "skb_cow_data" },
	{ 0xd1d6d87d, "pskb_expand_head" },
	{ 0xc5850110, "printk" },
	{ 0x37a02412, "xfrm_aalg_get_byname" },
	{ 0x2f95fc37, "crypto_ahash_setkey" },
	{ 0xf9b8eb3d, "crypto_alloc_ahash" },
	{ 0xf7dd9f37, "kmem_cache_alloc_trace" },
	{ 0xe18e5460, "kmalloc_caches" },
	{ 0xd2b09ce5, "__kmalloc" },
	{ 0x4e2380c8, "crypto_destroy_tfm" },
	{ 0x3536909f, "xfrm_output_resume" },
	{ 0x37a0cba, "kfree" },
	{ 0x69acdf38, "memcpy" },
	{ 0x83e58dc6, "ipv4_update_pmtu" },
	{ 0xf647b45c, "__xfrm_state_destroy" },
	{ 0x165b145c, "ex_handler_refcount" },
	{ 0xa99d147f, "ipv4_redirect" },
	{ 0x7bc9a3d1, "xfrm_state_lookup" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=xfrm_algo";


MODULE_INFO(srcversion, "2A22BB3D0909BDF8A27A7BB");
