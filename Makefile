# Makefile

include config.mak

vpath %.c $(SRCPATH)
vpath %.h $(SRCPATH)
vpath %.S $(SRCPATH)
vpath %.asm $(SRCPATH)
vpath %.rc $(SRCPATH)

all: default
default:

SRCS = common/common.c common/list.c common/scan.c common/mv.c common/cpu.c \
	   common/pattern.c common/bitstream.c common/set.c common/pic_sym.c \
	   common/simage.c common/image.c common/frame.c common/mc.c common/slice.c \
	   common/rd_cost_weight_prediction.c common/rd_cost.c \
	   common/transform.c common/tr_quant.c \
	   common/rate_dist_opt_quant_ssse3.c \
	   common/loop_filter.c \
	   common/motion_info.c common/sample_adaptive_offset.c \
	   common/tmvp_data_cu.c common/base_data_cu.c common/data_cu.c \
	   common/weight_prediction.c common/intra_pred.c common/prediction.c \
	   common/ip.c common/interpolation_filter.c \
	   common/x86/timer_test.c \
	   encoder/encoder.c encoder/enc_global.c \
	   encoder/enc_top.c encoder/enc_gop.c encoder/weight_pred_analysis.c \
	   encoder/enc_slice.c encoder/enc_cu.c \
	   encoder/set.c encoder/enc_bin_coder.c \
	   encoder/enc_bin_coder_cabac.c encoder/enc_bin_coder_cabac_counter.c \
	   encoder/enc_sbac.c encoder/enc_cavlc.c encoder/enc_entropy_if.c \
	   encoder/enc_entropy.c encoder/enc_search_i.c \
	   encoder/enc_search_pb_one.c encoder/enc_search_pb_two.c \
	   encoder/enc_sample_adaptive_offset.c encoder/enc_preanalyzer.c \
	   encoder/enc_analyze.c encoder/enc_rate_ctrl.c
SRCCLI = x265.c \
		config_file_parser.c print_parameters.c check_parameters.c \
		low_delay_config.c \
		frame_input/raw.c frame_input/frame_input.c frame_output/raw.c \
		bitstream_output/raw.c \
		input_filters/input_filters.c input_filters/source.c input_filters/resize.c input_filters/depth.c \
		output_filters/output_filters.c output_filters/source.c output_filters/resize.c output_filters/depth.c


SRCSO =
OBJS =
OBJSO =
OBJCLI =
OBJCHK =

CONFIG := $(shell cat config.h)


ifneq ($(AS),)
X86SRC0 = cpu-a.asm const-a.asm timer-a.asm \
			intra_pred_planar_ssse3_x86-a.asm \
			intra_pred_dc_ssse3_x86-a.asm \
			intra_pred_angle_0_8_ssse3_x86-a.asm \
			intra_pred_angle_1_8_ssse3_x86-a.asm \
			intra_pred_angle_2_4_8_ssse3_x86-a.asm \
			intra_pred_x86-a.asm \
			rdoq.asm \
			timer_test_x86-a.asm \
			rd_cost_hads_ssse3-a.asm \
			ip_coeff_ssse3_x86-a.asm \
			ip_hor_p_1_ssse3_x86-a.asm \
			ip_hor_p_2_ssse3_x86-a.asm \
			ip_ver_p_1_ssse3_x86-a.asm \
			ip_ver_p_2_ssse3_x86-a.asm \
			ip_ver_s_0_ssse3_x86-a.asm \
			transform_coeff_ssse3_x86-a.asm \
			transform_8_ssse3_x86-a.asm \
			transform_16_ssse3_x86-a.asm \
			transform_32_ssse3_x86-a.asm
X86SRC = $(X86SRC0:%=common/x86/%)


ifeq ($(ARCH),X86)
ARCH_X86 = yes
ASMSRC   = $(X86SRC)
ASFLAGS += -DARCH_X86_64=0
endif

ifdef ARCH_X86
ASFLAGS += -I$(SRCPATH)/common/x86/
OBJASM  = $(ASMSRC:%.asm=%.o)
$(OBJASM): common/x86/x86inc.asm common/x86/x86util.asm \
			common/x86/intra_pred_angle_ssse3_x86-a.asm \
			common/x86/intra_pred_angle_2_4_8_ssse3_x86-a.inc \
			common/x86/transform_1_3_ssse3_x86-a.inc \
			common/x86/ip_ssse3_x86-a.inc
OBJCHK += tools/checkasm-a.o
endif
endif

OBJS   += $(SRCS:%.c=%.o)
OBJCLI += $(SRCCLI:%.c=%.o)
OBJSO  += $(SRCSO:%.c=%.o)

cli: x265$(EXE)
$(LIBX265): .depend $(OBJS) $(OBJASM)
	rm -f $(LIBX265)
	$(AR)$@ $(OBJS) $(OBJASM)
	$(if $(RANLIB), $(RANLIB) $@)

x265$(EXE): .depend $(OBJCLI) $(CLI_LIBX265)
	$(LD)$@ $(OBJCLI) $(CLI_LIBX265) $(LDFLAGSCLI) $(LDFLAGS)
	
$(OBJS) $(OBJASM) $(OBJSO) $(OBJCLI) $(OBJCHK): .depend

%.o: %.asm
	$(AS) $(ASFLAGS) -o $@ $<
	-@ $(if $(STRIP), $(STRIP) -x $@) # delete local/anonymous symbols, so they don't show up in oprofile

.depend: config.mak
	@rm -f .depend
	@$(foreach SRC, $(addprefix $(SRCPATH)/, $(SRCS) $(SRCCLI) $(SRCSO)), $(CC) $(CFLAGS) $(SRC) $(DEPMT) $(SRC:$(SRCPATH)/%.c=%.o) $(DEPMM) 1>> .depend;)	

config.mak:
	./configure
	
depend: .depend
ifneq ($(wildcard .depend),)
include .depend
endif

	
clean:
	rm -f $(OBJS) $(OBJASM) $(OBJCLI) $(OBJSO) $(SONAME) *.a *.lib *.exp *.pdb x265 x265.exe .depend TAGS
	rm -f checkasm checkasm.exe $(OBJCHK)
	rm -f $(SRC2:%.c=%.gcda) $(SRC2:%.c=%.gcno) *.dyn pgopti.dpi pgopti.dpi.lock
	#rm -f config.h
	

	
