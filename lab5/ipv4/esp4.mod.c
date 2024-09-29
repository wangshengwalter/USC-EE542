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
	{ 0xd2b09ce5, "__kmalloc" },
	{ 0xf888ca21, "sg_init_table" },
	{ 0xf146d62b, "skb_to_sgvec" },
	{ 0xbff034b3, "__put_devmap_managed_page" },
	{ 0xe9bd3f25, "xfrm_unregister_type" },
	{ 0xb3635b01, "_raw_spin_lock_bh" },
	{ 0x83e58dc6, "ipv4_update_pmtu" },
	{ 0xf1f00d08, "xfrm4_protocol_deregister" },
	{ 0x837b7b09, "__dynamic_pr_debug" },
	{ 0x87b8798d, "sg_next" },
	{ 0x92ac667c, "pskb_put" },
	{ 0xe8eb8891, "skb_page_frag_refill" },
	{ 0x165b145c, "ex_handler_refcount" },
	{ 0x97651e6c, "vmemmap_base" },
	{ 0x122e64e0, "skb_cow_data" },
	{ 0x86dff270, "__pskb_pull_tail" },
	{ 0xa3327a1, "xfrm_input_resume" },
	{ 0xfb578fc5, "memset" },
	{ 0xd5ec2f80, "skb_checksum" },
	{ 0xfbca4909, "current_task" },
	{ 0xc5850110, "printk" },
	{ 0xf1e395a7, "xfrm_input" },
	{ 0xf4271b01, "skb_push" },
	{ 0x37a02412, "xfrm_aalg_get_byname" },
	{ 0xbf10083a, "crypto_aead_setkey" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0x24dff578, "xfrm_register_type" },
	{ 0x49c41a57, "_raw_spin_unlock_bh" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x9ef9fec2, "kfree_skb" },
	{ 0xa99d147f, "ipv4_redirect" },
	{ 0x2ea2c95c, "__x86_indirect_thunk_rax" },
	{ 0x4e2380c8, "crypto_destroy_tfm" },
	{ 0x3536909f, "xfrm_output_resume" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xb0b5ddf0, "skb_pull_rcsum" },
	{ 0xf647b45c, "__xfrm_state_destroy" },
	{ 0x4224d8b9, "km_new_mapping" },
	{ 0xf274c24d, "xfrm4_rcv" },
	{ 0xf6ebc03b, "net_ratelimit" },
	{ 0x2789f3, "xfrm4_protocol_register" },
	{ 0x82060889, "crypto_aead_setauthsize" },
	{ 0xac50bd7, "crypto_alloc_aead" },
	{ 0x7bc9a3d1, "xfrm_state_lookup" },
	{ 0x37a0cba, "kfree" },
	{ 0x69acdf38, "memcpy" },
	{ 0x7910fd7d, "___pskb_trim" },
	{ 0xa7461bd5, "__skb_ext_del" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x3001304a, "skb_copy_bits" },
	{ 0xec21e95a, "__put_page" },
	{ 0x587f22d7, "devmap_managed_key" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=xfrm_algo";


MODULE_INFO(srcversion, "3AA9F42C8D3889142D3D137");
