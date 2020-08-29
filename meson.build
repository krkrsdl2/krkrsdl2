project('krkrsdl2', ['c', 'cpp'], default_options: ['optimization=3', 'debug=true', 'warning_level=1'], version: 'unknown')

if target_machine.system() == 'darwin'
	add_languages('objcpp')
endif

cxx = meson.get_compiler('cpp')
host_system = host_machine.system()

sdl2_dep = dependency('sdl2', method: 'auto')

krkrsdl2_src = [
	'external/krkrz/base/BinaryStream.cpp',
	'external/krkrz/base/CharacterSet.cpp',
	'external/krkrz/base/EventIntf.cpp',
	'external/krkrz/base/PluginIntf.cpp',
	'external/krkrz/base/ScriptMgnIntf.cpp',
	'external/krkrz/base/StorageIntf.cpp',
	'external/krkrz/base/SysInitIntf.cpp',
	'external/krkrz/base/SystemIntf.cpp',
	'external/krkrz/base/TextStream.cpp',
	'external/krkrz/base/UtilStreams.cpp',
	'external/krkrz/base/XP3Archive.cpp',
	'external/krkrz/extension/Extension.cpp',
	'external/krkrz/msg/MsgIntf.cpp',
	'external/krkrz/sound/MathAlgorithms.cpp',
	'external/krkrz/sound/OpusCodecDecoder.cpp',
	'external/krkrz/sound/PhaseVocoderDSP.cpp',
	'external/krkrz/sound/PhaseVocoderFilter.cpp',
	'external/krkrz/sound/RealFFT.cpp',
	'external/krkrz/sound/SoundBufferBaseImpl.cpp',
	'external/krkrz/sound/SoundBufferBaseIntf.cpp',
	'external/krkrz/sound/WaveFormatConverter.cpp',
	'external/krkrz/sound/WaveIntf.cpp',
	'external/krkrz/sound/WaveLoopManager.cpp',
	'external/krkrz/sound/WaveSegmentQueue.cpp',
	'external/krkrz/sound/MathAlgorithms_SSE.cpp',
	'external/krkrz/sound/RealFFT_SSE.cpp',
	'external/krkrz/sound/WaveFormatConverter_SSE.cpp',
	'external/krkrz/sound/xmmlib.cpp',
	'external/krkrz/tjs2/tjs.cpp',
	'external/krkrz/tjs2/tjs.tab.cpp',
	'external/krkrz/tjs2/tjsArray.cpp',
	'external/krkrz/tjs2/tjsBinarySerializer.cpp',
	'external/krkrz/tjs2/tjsByteCodeLoader.cpp',
	'external/krkrz/tjs2/tjsCompileControl.cpp',
	'external/krkrz/tjs2/tjsConfig.cpp',
	'external/krkrz/tjs2/tjsConstArrayData.cpp',
	'external/krkrz/tjs2/tjsDate.cpp',
	'external/krkrz/tjs2/tjsDateParser.cpp',
	'external/krkrz/tjs2/tjsDebug.cpp',
	'external/krkrz/tjs2/tjsDictionary.cpp',
	'external/krkrz/tjs2/tjsDisassemble.cpp',
	'external/krkrz/tjs2/tjsError.cpp',
	'external/krkrz/tjs2/tjsException.cpp',
	'external/krkrz/tjs2/tjsGlobalStringMap.cpp',
	'external/krkrz/tjs2/tjsInterCodeExec.cpp',
	'external/krkrz/tjs2/tjsInterCodeGen.cpp',
	'external/krkrz/tjs2/tjsInterface.cpp',
	'external/krkrz/tjs2/tjsLex.cpp',
	'external/krkrz/tjs2/tjsMT19937ar-cok.cpp',
	'external/krkrz/tjs2/tjsMath.cpp',
	'external/krkrz/tjs2/tjsMessage.cpp',
	'external/krkrz/tjs2/tjsNamespace.cpp',
	'external/krkrz/tjs2/tjsNative.cpp',
	'external/krkrz/tjs2/tjsObject.cpp',
	'external/krkrz/tjs2/tjsObjectExtendable.cpp',
	'external/krkrz/tjs2/tjsOctPack.cpp',
	'external/krkrz/tjs2/tjsRandomGenerator.cpp',
	'external/krkrz/tjs2/tjsRegExp.cpp',
	'external/krkrz/tjs2/tjsScriptBlock.cpp',
	'external/krkrz/tjs2/tjsScriptCache.cpp',
	'external/krkrz/tjs2/tjsSnprintf.cpp',
	'external/krkrz/tjs2/tjsString.cpp',
	'external/krkrz/tjs2/tjsUtils.cpp',
	'external/krkrz/tjs2/tjsVariant.cpp',
	'external/krkrz/tjs2/tjsVariantString.cpp',
	'external/krkrz/tjs2/tjsdate.tab.cpp',
	'external/krkrz/tjs2/tjspp.tab.cpp',
	'external/krkrz/utils/ClipboardIntf.cpp',
	'external/krkrz/utils/DebugIntf.cpp',
	'external/krkrz/utils/MiscUtility.cpp',
	'external/krkrz/utils/Random.cpp',
	'external/krkrz/utils/TVPTimer.cpp',
	'external/krkrz/utils/ThreadIntf.cpp',
	'external/krkrz/utils/TickCount.cpp',
	'external/krkrz/utils/TimerIntf.cpp',
	'external/krkrz/utils/TimerThread.cpp',
	'external/krkrz/utils/VelocityTracker.cpp',
	'external/krkrz/utils/cp932_uni.cpp',
	'external/krkrz/utils/md5.c',
	'external/krkrz/utils/uni_cp932.cpp',
	'external/krkrz/visual/BitmapIntf.cpp',
	'external/krkrz/visual/BitmapLayerTreeOwner.cpp',
	'external/krkrz/visual/CharacterData.cpp',
	'external/krkrz/visual/ComplexRect.cpp',
	'external/krkrz/visual/DrawDevice.cpp',
	'external/krkrz/visual/FontSystem.cpp',
	'external/krkrz/visual/FreeType.cpp',
	'external/krkrz/visual/FreeTypeFontRasterizer.cpp',
	'external/krkrz/visual/GraphicsLoadThread.cpp',
	'external/krkrz/visual/GraphicsLoaderIntf.cpp',
	'external/krkrz/visual/ImageFunction.cpp',
	'external/krkrz/visual/LayerBitmapImpl.cpp',
	'external/krkrz/visual/LayerBitmapIntf.cpp',
	'external/krkrz/visual/LayerIntf.cpp',
	'external/krkrz/visual/LayerManager.cpp',
	'external/krkrz/visual/LayerTreeOwnerImpl.cpp',
	'external/krkrz/visual/LoadJPEG.cpp',
	'external/krkrz/visual/LoadPNG.cpp',
	'external/krkrz/visual/LoadTLG.cpp',
	'external/krkrz/visual/NullDrawDevice.cpp',
	'external/krkrz/visual/PrerenderedFont.cpp',
	'external/krkrz/visual/RectItf.cpp',
	'external/krkrz/visual/SaveTLG5.cpp',
	'external/krkrz/visual/SaveTLG6.cpp',
	'external/krkrz/visual/TransIntf.cpp',
	'external/krkrz/visual/VideoOvlIntf.cpp',
	'external/krkrz/visual/WindowIntf.cpp',
	'external/krkrz/visual/gl/ResampleImage.cpp',
	'external/krkrz/visual/gl/WeightFunctor.cpp',
	'external/krkrz/visual/gl/blend_function.cpp',
	'external/krkrz/visual/gl/ResampleImageAVX2.cpp',
	'external/krkrz/visual/gl/ResampleImageSSE2.cpp',
	'external/krkrz/visual/gl/adjust_color_sse2.cpp',
	'external/krkrz/visual/gl/blend_function_avx2.cpp',
	'external/krkrz/visual/gl/blend_function_sse2.cpp',
	'external/krkrz/visual/gl/boxblur_sse2.cpp',
	'external/krkrz/visual/gl/colorfill_sse2.cpp',
	'external/krkrz/visual/gl/colormap_sse2.cpp',
	'external/krkrz/visual/gl/pixelformat_sse2.cpp',
	'external/krkrz/visual/gl/tlg_sse2.cpp',
	'external/krkrz/visual/gl/univtrans_sse2.cpp',
	'external/krkrz/visual/gl/x86simdutil.cpp',
	'external/krkrz/visual/gl/x86simdutilAVX2.cpp',
	'external/krkrz/visual/tvpgl.c',
	'external/krkrz/external/freetype/src/autofit/autofit.c',
	'external/krkrz/external/freetype/src/base/ftbase.c',
	'external/krkrz/external/freetype/src/base/ftbbox.c',
	'external/krkrz/external/freetype/src/base/ftbitmap.c',
	'external/krkrz/external/freetype/src/base/ftfntfmt.c',
	'external/krkrz/external/freetype/src/base/ftfstype.c',
	'external/krkrz/external/freetype/src/base/ftgasp.c',
	'external/krkrz/external/freetype/src/base/ftglyph.c',
	'external/krkrz/external/freetype/src/base/ftgxval.c',
	'external/krkrz/external/freetype/src/base/ftinit.c',
	'external/krkrz/external/freetype/src/base/ftlcdfil.c',
	'external/krkrz/external/freetype/src/base/ftmm.c',
	'external/krkrz/external/freetype/src/base/ftotval.c',
	'external/krkrz/external/freetype/src/base/ftpatent.c',
	'external/krkrz/external/freetype/src/base/ftpfr.c',
	'external/krkrz/external/freetype/src/base/ftstroke.c',
	'external/krkrz/external/freetype/src/base/ftsynth.c',
	'external/krkrz/external/freetype/src/base/ftsystem.c',
	'external/krkrz/external/freetype/src/base/fttype1.c',
	'external/krkrz/external/freetype/src/base/ftwinfnt.c',
	'external/krkrz/external/freetype/src/bdf/bdf.c',
	'external/krkrz/external/freetype/src/cache/ftcache.c',
	'external/krkrz/external/freetype/src/cff/cff.c',
	'external/krkrz/external/freetype/src/cid/type1cid.c',
	'external/krkrz/external/freetype/src/gzip/ftgzip.c',
	'external/krkrz/external/freetype/src/lzw/ftlzw.c',
	'external/krkrz/external/freetype/src/pcf/pcf.c',
	'external/krkrz/external/freetype/src/pfr/pfr.c',
	'external/krkrz/external/freetype/src/psaux/psaux.c',
	'external/krkrz/external/freetype/src/pshinter/pshinter.c',
	'external/krkrz/external/freetype/src/psnames/psmodule.c',
	'external/krkrz/external/freetype/src/raster/raster.c',
	'external/krkrz/external/freetype/src/sfnt/sfnt.c',
	'external/krkrz/external/freetype/src/smooth/smooth.c',
	'external/krkrz/external/freetype/src/truetype/truetype.c',
	'external/krkrz/external/freetype/src/type1/type1.c',
	'external/krkrz/external/freetype/src/type42/type42.c',
	'external/krkrz/external/freetype/src/winfonts/winfnt.c',
	'external/krkrz/external/libjpeg-turbo/jaricom.c',
	'external/krkrz/external/libjpeg-turbo/jcapimin.c',
	'external/krkrz/external/libjpeg-turbo/jcapistd.c',
	'external/krkrz/external/libjpeg-turbo/jcarith.c',
	'external/krkrz/external/libjpeg-turbo/jccoefct.c',
	'external/krkrz/external/libjpeg-turbo/jccolor.c',
	'external/krkrz/external/libjpeg-turbo/jcdctmgr.c',
	'external/krkrz/external/libjpeg-turbo/jchuff.c',
	'external/krkrz/external/libjpeg-turbo/jcinit.c',
	'external/krkrz/external/libjpeg-turbo/jcmainct.c',
	'external/krkrz/external/libjpeg-turbo/jcmarker.c',
	'external/krkrz/external/libjpeg-turbo/jcmaster.c',
	'external/krkrz/external/libjpeg-turbo/jcomapi.c',
	'external/krkrz/external/libjpeg-turbo/jcparam.c',
	'external/krkrz/external/libjpeg-turbo/jcphuff.c',
	'external/krkrz/external/libjpeg-turbo/jcprepct.c',
	'external/krkrz/external/libjpeg-turbo/jcsample.c',
	'external/krkrz/external/libjpeg-turbo/jctrans.c',
	'external/krkrz/external/libjpeg-turbo/jdapimin.c',
	'external/krkrz/external/libjpeg-turbo/jdapistd.c',
	'external/krkrz/external/libjpeg-turbo/jdarith.c',
	'external/krkrz/external/libjpeg-turbo/jdatadst-tj.c',
	'external/krkrz/external/libjpeg-turbo/jdatadst.c',
	'external/krkrz/external/libjpeg-turbo/jdatasrc-tj.c',
	'external/krkrz/external/libjpeg-turbo/jdatasrc.c',
	'external/krkrz/external/libjpeg-turbo/jdcoefct.c',
	'external/krkrz/external/libjpeg-turbo/jdcolor.c',
	'external/krkrz/external/libjpeg-turbo/jddctmgr.c',
	'external/krkrz/external/libjpeg-turbo/jdhuff.c',
	'external/krkrz/external/libjpeg-turbo/jdinput.c',
	'external/krkrz/external/libjpeg-turbo/jdmainct.c',
	'external/krkrz/external/libjpeg-turbo/jdmarker.c',
	'external/krkrz/external/libjpeg-turbo/jdmaster.c',
	'external/krkrz/external/libjpeg-turbo/jdmerge.c',
	'external/krkrz/external/libjpeg-turbo/jdphuff.c',
	'external/krkrz/external/libjpeg-turbo/jdpostct.c',
	'external/krkrz/external/libjpeg-turbo/jdsample.c',
	'external/krkrz/external/libjpeg-turbo/jdtrans.c',
	'external/krkrz/external/libjpeg-turbo/jerror.c',
	'external/krkrz/external/libjpeg-turbo/jfdctflt.c',
	'external/krkrz/external/libjpeg-turbo/jfdctfst.c',
	'external/krkrz/external/libjpeg-turbo/jfdctint.c',
	'external/krkrz/external/libjpeg-turbo/jidctflt.c',
	'external/krkrz/external/libjpeg-turbo/jidctfst.c',
	'external/krkrz/external/libjpeg-turbo/jidctint.c',
	'external/krkrz/external/libjpeg-turbo/jidctred.c',
	'external/krkrz/external/libjpeg-turbo/jmemmgr.c',
	'external/krkrz/external/libjpeg-turbo/jmemnobs.c',
	'external/krkrz/external/libjpeg-turbo/jquant1.c',
	'external/krkrz/external/libjpeg-turbo/jquant2.c',
	'external/krkrz/external/libjpeg-turbo/jsimd_none.c',
	'external/krkrz/external/libjpeg-turbo/jutils.c',
	'external/krkrz/external/libjpeg-turbo/transupp.c',
	'external/krkrz/external/libjpeg-turbo/turbojpeg.c',
	'external/krkrz/external/libogg/src/bitwise.c',
	'external/krkrz/external/libogg/src/framing.c',
	'external/krkrz/external/lpng/png.c',
	'external/krkrz/external/lpng/pngerror.c',
	'external/krkrz/external/lpng/pngget.c',
	'external/krkrz/external/lpng/pngmem.c',
	'external/krkrz/external/lpng/pngpread.c',
	'external/krkrz/external/lpng/pngread.c',
	'external/krkrz/external/lpng/pngrio.c',
	'external/krkrz/external/lpng/pngrtran.c',
	'external/krkrz/external/lpng/pngrutil.c',
	'external/krkrz/external/lpng/pngset.c',
	'external/krkrz/external/lpng/pngtrans.c',
	'external/krkrz/external/lpng/pngwio.c',
	'external/krkrz/external/lpng/pngwrite.c',
	'external/krkrz/external/lpng/pngwtran.c',
	'external/krkrz/external/lpng/pngwutil.c',
	'external/krkrz/external/onig/src/ascii.c',
	'external/krkrz/external/onig/src/big5.c',
	'external/krkrz/external/onig/src/cp1251.c',
	'external/krkrz/external/onig/src/euc_jp.c',
	'external/krkrz/external/onig/src/euc_jp_prop.c',
	'external/krkrz/external/onig/src/euc_kr.c',
	'external/krkrz/external/onig/src/euc_tw.c',
	'external/krkrz/external/onig/src/gb18030.c',
	'external/krkrz/external/onig/src/iso8859_1.c',
	'external/krkrz/external/onig/src/iso8859_10.c',
	'external/krkrz/external/onig/src/iso8859_11.c',
	'external/krkrz/external/onig/src/iso8859_13.c',
	'external/krkrz/external/onig/src/iso8859_14.c',
	'external/krkrz/external/onig/src/iso8859_15.c',
	'external/krkrz/external/onig/src/iso8859_16.c',
	'external/krkrz/external/onig/src/iso8859_2.c',
	'external/krkrz/external/onig/src/iso8859_3.c',
	'external/krkrz/external/onig/src/iso8859_4.c',
	'external/krkrz/external/onig/src/iso8859_5.c',
	'external/krkrz/external/onig/src/iso8859_6.c',
	'external/krkrz/external/onig/src/iso8859_7.c',
	'external/krkrz/external/onig/src/iso8859_8.c',
	'external/krkrz/external/onig/src/iso8859_9.c',
	'external/krkrz/external/onig/src/koi8_r.c',
	'external/krkrz/external/onig/src/onig_init.c',
	'external/krkrz/external/onig/src/regcomp.c',
	'external/krkrz/external/onig/src/regenc.c',
	'external/krkrz/external/onig/src/regerror.c',
	'external/krkrz/external/onig/src/regexec.c',
	'external/krkrz/external/onig/src/regext.c',
	'external/krkrz/external/onig/src/reggnu.c',
	'external/krkrz/external/onig/src/regparse.c',
	'external/krkrz/external/onig/src/regposerr.c',
	'external/krkrz/external/onig/src/regposix.c',
	'external/krkrz/external/onig/src/regsyntax.c',
	'external/krkrz/external/onig/src/regtrav.c',
	'external/krkrz/external/onig/src/regversion.c',
	'external/krkrz/external/onig/src/sjis.c',
	'external/krkrz/external/onig/src/sjis_prop.c',
	'external/krkrz/external/onig/src/st.c',
	'external/krkrz/external/onig/src/unicode.c',
	'external/krkrz/external/onig/src/unicode_fold1_key.c',
	'external/krkrz/external/onig/src/unicode_fold2_key.c',
	'external/krkrz/external/onig/src/unicode_fold3_key.c',
	'external/krkrz/external/onig/src/unicode_unfold_key.c',
	'external/krkrz/external/onig/src/utf16_be.c',
	'external/krkrz/external/onig/src/utf16_le.c',
	'external/krkrz/external/onig/src/utf32_be.c',
	'external/krkrz/external/onig/src/utf32_le.c',
	'external/krkrz/external/onig/src/utf8.c',
	'external/krkrz/external/opus/celt/bands.c',
	'external/krkrz/external/opus/celt/celt.c',
	'external/krkrz/external/opus/celt/celt_decoder.c',
	'external/krkrz/external/opus/celt/celt_encoder.c',
	'external/krkrz/external/opus/celt/celt_lpc.c',
	'external/krkrz/external/opus/celt/cwrs.c',
	'external/krkrz/external/opus/celt/entcode.c',
	'external/krkrz/external/opus/celt/entdec.c',
	'external/krkrz/external/opus/celt/entenc.c',
	'external/krkrz/external/opus/celt/kiss_fft.c',
	'external/krkrz/external/opus/celt/laplace.c',
	'external/krkrz/external/opus/celt/mathops.c',
	'external/krkrz/external/opus/celt/mdct.c',
	'external/krkrz/external/opus/celt/modes.c',
	'external/krkrz/external/opus/celt/pitch.c',
	'external/krkrz/external/opus/celt/quant_bands.c',
	'external/krkrz/external/opus/celt/rate.c',
	'external/krkrz/external/opus/celt/vq.c',
	'external/krkrz/external/opus/silk/A2NLSF.c',
	'external/krkrz/external/opus/silk/CNG.c',
	'external/krkrz/external/opus/silk/HP_variable_cutoff.c',
	'external/krkrz/external/opus/silk/LPC_analysis_filter.c',
	'external/krkrz/external/opus/silk/LPC_inv_pred_gain.c',
	'external/krkrz/external/opus/silk/LP_variable_cutoff.c',
	'external/krkrz/external/opus/silk/NLSF2A.c',
	'external/krkrz/external/opus/silk/NLSF_VQ.c',
	'external/krkrz/external/opus/silk/NLSF_VQ_weights_laroia.c',
	'external/krkrz/external/opus/silk/NLSF_decode.c',
	'external/krkrz/external/opus/silk/NLSF_del_dec_quant.c',
	'external/krkrz/external/opus/silk/NLSF_encode.c',
	'external/krkrz/external/opus/silk/NLSF_stabilize.c',
	'external/krkrz/external/opus/silk/NLSF_unpack.c',
	'external/krkrz/external/opus/silk/NSQ.c',
	'external/krkrz/external/opus/silk/NSQ_del_dec.c',
	'external/krkrz/external/opus/silk/PLC.c',
	'external/krkrz/external/opus/silk/VAD.c',
	'external/krkrz/external/opus/silk/VQ_WMat_EC.c',
	'external/krkrz/external/opus/silk/ana_filt_bank_1.c',
	'external/krkrz/external/opus/silk/biquad_alt.c',
	'external/krkrz/external/opus/silk/bwexpander.c',
	'external/krkrz/external/opus/silk/bwexpander_32.c',
	'external/krkrz/external/opus/silk/check_control_input.c',
	'external/krkrz/external/opus/silk/code_signs.c',
	'external/krkrz/external/opus/silk/control_SNR.c',
	'external/krkrz/external/opus/silk/control_audio_bandwidth.c',
	'external/krkrz/external/opus/silk/control_codec.c',
	'external/krkrz/external/opus/silk/debug.c',
	'external/krkrz/external/opus/silk/dec_API.c',
	'external/krkrz/external/opus/silk/decode_core.c',
	'external/krkrz/external/opus/silk/decode_frame.c',
	'external/krkrz/external/opus/silk/decode_indices.c',
	'external/krkrz/external/opus/silk/decode_parameters.c',
	'external/krkrz/external/opus/silk/decode_pitch.c',
	'external/krkrz/external/opus/silk/decode_pulses.c',
	'external/krkrz/external/opus/silk/decoder_set_fs.c',
	'external/krkrz/external/opus/silk/enc_API.c',
	'external/krkrz/external/opus/silk/encode_indices.c',
	'external/krkrz/external/opus/silk/encode_pulses.c',
	'external/krkrz/external/opus/silk/float/LPC_analysis_filter_FLP.c',
	'external/krkrz/external/opus/silk/float/LPC_inv_pred_gain_FLP.c',
	'external/krkrz/external/opus/silk/float/LTP_analysis_filter_FLP.c',
	'external/krkrz/external/opus/silk/float/LTP_scale_ctrl_FLP.c',
	'external/krkrz/external/opus/silk/float/apply_sine_window_FLP.c',
	'external/krkrz/external/opus/silk/float/autocorrelation_FLP.c',
	'external/krkrz/external/opus/silk/float/burg_modified_FLP.c',
	'external/krkrz/external/opus/silk/float/bwexpander_FLP.c',
	'external/krkrz/external/opus/silk/float/corrMatrix_FLP.c',
	'external/krkrz/external/opus/silk/float/encode_frame_FLP.c',
	'external/krkrz/external/opus/silk/float/energy_FLP.c',
	'external/krkrz/external/opus/silk/float/find_LPC_FLP.c',
	'external/krkrz/external/opus/silk/float/find_LTP_FLP.c',
	'external/krkrz/external/opus/silk/float/find_pitch_lags_FLP.c',
	'external/krkrz/external/opus/silk/float/find_pred_coefs_FLP.c',
	'external/krkrz/external/opus/silk/float/inner_product_FLP.c',
	'external/krkrz/external/opus/silk/float/k2a_FLP.c',
	'external/krkrz/external/opus/silk/float/levinsondurbin_FLP.c',
	'external/krkrz/external/opus/silk/float/noise_shape_analysis_FLP.c',
	'external/krkrz/external/opus/silk/float/pitch_analysis_core_FLP.c',
	'external/krkrz/external/opus/silk/float/prefilter_FLP.c',
	'external/krkrz/external/opus/silk/float/process_gains_FLP.c',
	'external/krkrz/external/opus/silk/float/regularize_correlations_FLP.c',
	'external/krkrz/external/opus/silk/float/residual_energy_FLP.c',
	'external/krkrz/external/opus/silk/float/scale_copy_vector_FLP.c',
	'external/krkrz/external/opus/silk/float/scale_vector_FLP.c',
	'external/krkrz/external/opus/silk/float/schur_FLP.c',
	'external/krkrz/external/opus/silk/float/solve_LS_FLP.c',
	'external/krkrz/external/opus/silk/float/sort_FLP.c',
	'external/krkrz/external/opus/silk/float/warped_autocorrelation_FLP.c',
	'external/krkrz/external/opus/silk/float/wrappers_FLP.c',
	'external/krkrz/external/opus/silk/gain_quant.c',
	'external/krkrz/external/opus/silk/init_decoder.c',
	'external/krkrz/external/opus/silk/init_encoder.c',
	'external/krkrz/external/opus/silk/inner_prod_aligned.c',
	'external/krkrz/external/opus/silk/interpolate.c',
	'external/krkrz/external/opus/silk/lin2log.c',
	'external/krkrz/external/opus/silk/log2lin.c',
	'external/krkrz/external/opus/silk/pitch_est_tables.c',
	'external/krkrz/external/opus/silk/process_NLSFs.c',
	'external/krkrz/external/opus/silk/quant_LTP_gains.c',
	'external/krkrz/external/opus/silk/resampler.c',
	'external/krkrz/external/opus/silk/resampler_down2.c',
	'external/krkrz/external/opus/silk/resampler_down2_3.c',
	'external/krkrz/external/opus/silk/resampler_private_AR2.c',
	'external/krkrz/external/opus/silk/resampler_private_IIR_FIR.c',
	'external/krkrz/external/opus/silk/resampler_private_down_FIR.c',
	'external/krkrz/external/opus/silk/resampler_private_up2_HQ.c',
	'external/krkrz/external/opus/silk/resampler_rom.c',
	'external/krkrz/external/opus/silk/shell_coder.c',
	'external/krkrz/external/opus/silk/sigm_Q15.c',
	'external/krkrz/external/opus/silk/sort.c',
	'external/krkrz/external/opus/silk/stereo_LR_to_MS.c',
	'external/krkrz/external/opus/silk/stereo_MS_to_LR.c',
	'external/krkrz/external/opus/silk/stereo_decode_pred.c',
	'external/krkrz/external/opus/silk/stereo_encode_pred.c',
	'external/krkrz/external/opus/silk/stereo_find_predictor.c',
	'external/krkrz/external/opus/silk/stereo_quant_pred.c',
	'external/krkrz/external/opus/silk/sum_sqr_shift.c',
	'external/krkrz/external/opus/silk/table_LSF_cos.c',
	'external/krkrz/external/opus/silk/tables_LTP.c',
	'external/krkrz/external/opus/silk/tables_NLSF_CB_NB_MB.c',
	'external/krkrz/external/opus/silk/tables_NLSF_CB_WB.c',
	'external/krkrz/external/opus/silk/tables_gain.c',
	'external/krkrz/external/opus/silk/tables_other.c',
	'external/krkrz/external/opus/silk/tables_pitch_lag.c',
	'external/krkrz/external/opus/silk/tables_pulses_per_block.c',
	'external/krkrz/external/opus/src/analysis.c',
	'external/krkrz/external/opus/src/mlp.c',
	'external/krkrz/external/opus/src/mlp_data.c',
	'external/krkrz/external/opus/src/opus.c',
	'external/krkrz/external/opus/src/opus_decoder.c',
	'external/krkrz/external/opus/src/opus_encoder.c',
	'external/krkrz/external/opus/src/opus_multistream.c',
	'external/krkrz/external/opus/src/opus_multistream_decoder.c',
	'external/krkrz/external/opus/src/opus_multistream_encoder.c',
	'external/krkrz/external/opus/src/repacketizer.c',
	'external/krkrz/external/opusfile/src/http.c',
	'external/krkrz/external/opusfile/src/info.c',
	'external/krkrz/external/opusfile/src/internal.c',
	'external/krkrz/external/opusfile/src/opusfile.c',
	'external/krkrz/external/opusfile/src/stream.c',
	'external/krkrz/external/opusfile/src/wincerts.c',
	'external/krkrz/external/zlib/adler32.c',
	'external/krkrz/external/zlib/compress.c',
	'external/krkrz/external/zlib/crc32.c',
	'external/krkrz/external/zlib/deflate.c',
	'external/krkrz/external/zlib/infback.c',
	'external/krkrz/external/zlib/inffast.c',
	'external/krkrz/external/zlib/inflate.c',
	'external/krkrz/external/zlib/inftrees.c',
	'external/krkrz/external/zlib/trees.c',
	'external/krkrz/external/zlib/uncompr.c',
	'external/krkrz/external/zlib/zutil.c',
	'external/ncbind/ncbind.cpp',
	'src/core/base/sdl2/EventImpl.cpp',
	'src/core/base/sdl2/FileSelector.cpp',
	'src/core/base/sdl2/NativeEventQueue.cpp',
	'src/core/base/sdl2/PluginImpl.cpp',
	'src/core/base/sdl2/ScriptMgnImpl.cpp',
	'src/core/base/sdl2/StorageImpl.cpp',
	'src/core/base/sdl2/SysInitImpl.cpp',
	'src/core/base/sdl2/SystemImpl.cpp',
	'src/core/environ/sdl2/Application.cpp',
	'src/core/environ/sdl2/DetectCPU.cpp',
	'src/core/environ/sdl2/SystemControl.cpp',
	'src/core/msg/sdl2/MsgImpl.cpp',
	'src/core/msg/sdl2/MsgLoad.cpp',
	'src/core/sdl2/SDLApplication.cpp',
	'src/core/sdl2/SDLBitmapCompletion.cpp',
	'src/core/sound/sdl2/WaveImpl.cpp',
	'src/core/sound/sdl2/WaveMixer.cpp',
	'src/core/utils/sdl2/ClipboardImpl.cpp',
	'src/core/visual/sdl2/BasicDrawDevice.cpp',
	'src/core/visual/sdl2/BitmapBitsAlloc.cpp',
	'src/core/visual/sdl2/BitmapInfomation.cpp',
	'src/core/visual/sdl2/GraphicsLoaderImpl.cpp',
	'src/core/visual/sdl2/LayerImpl.cpp',
	'src/core/visual/sdl2/TVPScreen.cpp',
	'src/core/visual/sdl2/VideoOvlImpl.cpp',
	'src/core/visual/sdl2/WindowImpl.cpp',
]
krkrsdl2_includes = [
	'src/core/base/sdl2',
	'src/core/environ/sdl2',
	'src/core/msg/sdl2',
	'src/core/sdl2',
	'src/core/sound/sdl2',
	'src/core/utils/sdl2',
	'src/core/visual/sdl2',
	'external',
	'external/krkrz',
	'external/krkrz/base',
	'external/krkrz/environ',
	'external/krkrz/extension',
	'external/krkrz/msg',
	'external/krkrz/sound',
	'external/krkrz/tjs2',
	'external/krkrz/utils',
	'external/krkrz/visual',
	'external/krkrz/visual/gl',
	'external/krkrz/visual/IA32',
	'external/krkrz/external',
	'external/krkrz/external/freetype/include',
	'external/krkrz/external/freetype/src',
	'external/krkrz/external/libjpeg-turbo',
	'external/krkrz/external/libogg/include',
	'external/krkrz/external/lpng',
	'external/krkrz/external/onig/src',
	'external/krkrz/external/libogg/include',
	'external/krkrz/external/opus/celt',
	'external/krkrz/external/opus/include',
	'external/krkrz/external/opus/silk',
	'external/krkrz/external/opus/silk/fixed',
	'external/krkrz/external/opus/silk/float',
	'external/krkrz/external/opusfile/include',
	'external/krkrz/external/opusfile/src',
	'external/krkrz/external/zlib',
	'src/config',
	'src/plugins',
]
if get_option('builtin_plugin_enable_kagparser') == true
	krkrsdl2_src += [
		'external/KAGParser/KAGParser.cpp',
	]
endif
if get_option('builtin_plugin_enable_extrans') == true
	krkrsdl2_src += [
		'external/SamplePlugin/extrans/Main.cpp',
		'external/SamplePlugin/extrans/mosaic.cpp',
		'external/SamplePlugin/extrans/ripple.cpp',
		'external/SamplePlugin/extrans/rotatebase.cpp',
		'external/SamplePlugin/extrans/rotatetrans.cpp',
		'external/SamplePlugin/extrans/turn.cpp',
		'external/SamplePlugin/extrans/turntrans_table.cpp',
		'external/SamplePlugin/extrans/wave.cpp',
	]
endif
if get_option('builtin_plugin_enable_extrans') == true
	krkrsdl2_src += [
		'external/SamplePlugin/wutcwf/WUMainUnit.cpp',
	]
endif
if get_option('builtin_plugin_enable_csvparser') == true
	krkrsdl2_src += [
		'external/csvParser/Main.cpp',
	]
endif
if get_option('builtin_plugin_enable_fstat') == true
	krkrsdl2_src += [
		'external/fstat/Main.cpp',
	]
endif
if get_option('builtin_plugin_enable_getsample') == true
	krkrsdl2_src += [
		'external/getSample/main.cpp',
	]
endif
if get_option('builtin_plugin_enable_json') == true
	krkrsdl2_src += [
		'external/json/Main.cpp',
	]
endif
if get_option('builtin_plugin_enable_layerexbtoa') == true
	krkrsdl2_src += [
		'external/layerExBTOA/Main.cpp',
	]
endif
if get_option('builtin_plugin_enable_layereximage') == true
	krkrsdl2_src += [
		'external/layerExImage/LayerExImage.cpp',
		'external/layerExImage/Main.cpp',
	]
endif
if get_option('builtin_plugin_enable_wuvorbis') == true
	krkrsdl2_src += [
		'external/wuvorbis/WuVorbisMainUnit.cpp',
		'external/wuvorbis/external/vorbis/lib/bitrate.c',
		'external/wuvorbis/external/vorbis/lib/block.c',
		'external/wuvorbis/external/vorbis/lib/codebook.c',
		'external/wuvorbis/external/vorbis/lib/envelope.c',
		'external/wuvorbis/external/vorbis/lib/floor0.c',
		'external/wuvorbis/external/vorbis/lib/floor1.c',
		'external/wuvorbis/external/vorbis/lib/info.c',
		'external/wuvorbis/external/vorbis/lib/lpc.c',
		'external/wuvorbis/external/vorbis/lib/lsp.c',
		'external/wuvorbis/external/vorbis/lib/mapping0.c',
		'external/wuvorbis/external/vorbis/lib/mdct.c',
		'external/wuvorbis/external/vorbis/lib/psy.c',
		'external/wuvorbis/external/vorbis/lib/registry.c',
		'external/wuvorbis/external/vorbis/lib/res0.c',
		'external/wuvorbis/external/vorbis/lib/sharedbook.c',
		'external/wuvorbis/external/vorbis/lib/smallft.c',
		'external/wuvorbis/external/vorbis/lib/synthesis.c',
		'external/wuvorbis/external/vorbis/lib/vorbisfile.c',
		'external/wuvorbis/external/vorbis/lib/window.c',
	]
	krkrsdl2_includes += [
		'external/wuvorbis/external/vorbis/include',
	]
endif
if get_option('builtin_plugin_enable_kratrac9') == true
	krkrsdl2_src += [
		'external/kratrac9/kratrac9/at9wave.cpp',
		'external/kratrac9/kratrac9/Atrac9MainUnit.cpp',
		'external/kratrac9/libatrac9/src/band_extension.c',
		'external/kratrac9/libatrac9/src/bit_allocation.c',
		'external/kratrac9/libatrac9/src/bit_reader.c',
		'external/kratrac9/libatrac9/src/decinit.c',
		'external/kratrac9/libatrac9/src/decoder.c',
		'external/kratrac9/libatrac9/src/huffCodes.c',
		'external/kratrac9/libatrac9/src/imdct.c',
		'external/kratrac9/libatrac9/src/libatrac9.c',
		'external/kratrac9/libatrac9/src/quantization.c',
		'external/kratrac9/libatrac9/src/scale_factors.c',
		'external/kratrac9/libatrac9/src/tables.c',
		'external/kratrac9/libatrac9/src/unpack.c',
		'external/kratrac9/libatrac9/src/utility.c',
	]
endif
if get_option('builtin_plugin_enable_scriptsex') == true
	krkrsdl2_src += [
		'external/krglhwebp/dllmain.cpp',
		'external/krglhwebp/libwebp/src/dec/alpha_dec.c',
		'external/krglhwebp/libwebp/src/dec/buffer_dec.c',
		'external/krglhwebp/libwebp/src/dec/frame_dec.c',
		'external/krglhwebp/libwebp/src/dec/idec_dec.c',
		'external/krglhwebp/libwebp/src/dec/io_dec.c',
		'external/krglhwebp/libwebp/src/dec/quant_dec.c',
		'external/krglhwebp/libwebp/src/dec/tree_dec.c',
		'external/krglhwebp/libwebp/src/dec/vp8_dec.c',
		'external/krglhwebp/libwebp/src/dec/vp8l_dec.c',
		'external/krglhwebp/libwebp/src/dec/webp_dec.c',
		'external/krglhwebp/libwebp/src/demux/anim_decode.c',
		'external/krglhwebp/libwebp/src/demux/demux.c',
		'external/krglhwebp/libwebp/src/dsp/alpha_processing.c',
		'external/krglhwebp/libwebp/src/dsp/alpha_processing_mips_dsp_r2.c',
		'external/krglhwebp/libwebp/src/dsp/alpha_processing_neon.c',
		'external/krglhwebp/libwebp/src/dsp/alpha_processing_sse2.c',
		'external/krglhwebp/libwebp/src/dsp/alpha_processing_sse41.c',
		'external/krglhwebp/libwebp/src/dsp/cost.c',
		'external/krglhwebp/libwebp/src/dsp/cost_mips32.c',
		'external/krglhwebp/libwebp/src/dsp/cost_mips_dsp_r2.c',
		'external/krglhwebp/libwebp/src/dsp/cost_neon.c',
		'external/krglhwebp/libwebp/src/dsp/cost_sse2.c',
		'external/krglhwebp/libwebp/src/dsp/cpu.c',
		'external/krglhwebp/libwebp/src/dsp/dec.c',
		'external/krglhwebp/libwebp/src/dsp/dec_clip_tables.c',
		'external/krglhwebp/libwebp/src/dsp/dec_mips32.c',
		'external/krglhwebp/libwebp/src/dsp/dec_mips_dsp_r2.c',
		'external/krglhwebp/libwebp/src/dsp/dec_msa.c',
		'external/krglhwebp/libwebp/src/dsp/dec_neon.c',
		'external/krglhwebp/libwebp/src/dsp/dec_sse2.c',
		'external/krglhwebp/libwebp/src/dsp/dec_sse41.c',
		'external/krglhwebp/libwebp/src/dsp/enc.c',
		'external/krglhwebp/libwebp/src/dsp/enc_mips32.c',
		'external/krglhwebp/libwebp/src/dsp/enc_mips_dsp_r2.c',
		'external/krglhwebp/libwebp/src/dsp/enc_msa.c',
		'external/krglhwebp/libwebp/src/dsp/enc_neon.c',
		'external/krglhwebp/libwebp/src/dsp/enc_sse2.c',
		'external/krglhwebp/libwebp/src/dsp/enc_sse41.c',
		'external/krglhwebp/libwebp/src/dsp/filters.c',
		'external/krglhwebp/libwebp/src/dsp/filters_mips_dsp_r2.c',
		'external/krglhwebp/libwebp/src/dsp/filters_msa.c',
		'external/krglhwebp/libwebp/src/dsp/filters_neon.c',
		'external/krglhwebp/libwebp/src/dsp/filters_sse2.c',
		'external/krglhwebp/libwebp/src/dsp/lossless.c',
		'external/krglhwebp/libwebp/src/dsp/lossless_enc.c',
		'external/krglhwebp/libwebp/src/dsp/lossless_enc_mips32.c',
		'external/krglhwebp/libwebp/src/dsp/lossless_enc_mips_dsp_r2.c',
		'external/krglhwebp/libwebp/src/dsp/lossless_enc_msa.c',
		'external/krglhwebp/libwebp/src/dsp/lossless_enc_neon.c',
		'external/krglhwebp/libwebp/src/dsp/lossless_enc_sse2.c',
		'external/krglhwebp/libwebp/src/dsp/lossless_enc_sse41.c',
		'external/krglhwebp/libwebp/src/dsp/lossless_mips_dsp_r2.c',
		'external/krglhwebp/libwebp/src/dsp/lossless_msa.c',
		'external/krglhwebp/libwebp/src/dsp/lossless_neon.c',
		'external/krglhwebp/libwebp/src/dsp/lossless_sse2.c',
		'external/krglhwebp/libwebp/src/dsp/rescaler.c',
		'external/krglhwebp/libwebp/src/dsp/rescaler_mips32.c',
		'external/krglhwebp/libwebp/src/dsp/rescaler_mips_dsp_r2.c',
		'external/krglhwebp/libwebp/src/dsp/rescaler_msa.c',
		'external/krglhwebp/libwebp/src/dsp/rescaler_neon.c',
		'external/krglhwebp/libwebp/src/dsp/rescaler_sse2.c',
		'external/krglhwebp/libwebp/src/dsp/ssim.c',
		'external/krglhwebp/libwebp/src/dsp/ssim_sse2.c',
		'external/krglhwebp/libwebp/src/dsp/upsampling.c',
		'external/krglhwebp/libwebp/src/dsp/upsampling_mips_dsp_r2.c',
		'external/krglhwebp/libwebp/src/dsp/upsampling_msa.c',
		'external/krglhwebp/libwebp/src/dsp/upsampling_neon.c',
		'external/krglhwebp/libwebp/src/dsp/upsampling_sse2.c',
		'external/krglhwebp/libwebp/src/dsp/upsampling_sse41.c',
		'external/krglhwebp/libwebp/src/dsp/yuv.c',
		'external/krglhwebp/libwebp/src/dsp/yuv_mips32.c',
		'external/krglhwebp/libwebp/src/dsp/yuv_mips_dsp_r2.c',
		'external/krglhwebp/libwebp/src/dsp/yuv_neon.c',
		'external/krglhwebp/libwebp/src/dsp/yuv_sse2.c',
		'external/krglhwebp/libwebp/src/dsp/yuv_sse41.c',
		'external/krglhwebp/libwebp/src/enc/alpha_enc.c',
		'external/krglhwebp/libwebp/src/enc/analysis_enc.c',
		'external/krglhwebp/libwebp/src/enc/backward_references_cost_enc.c',
		'external/krglhwebp/libwebp/src/enc/backward_references_enc.c',
		'external/krglhwebp/libwebp/src/enc/config_enc.c',
		'external/krglhwebp/libwebp/src/enc/cost_enc.c',
		'external/krglhwebp/libwebp/src/enc/filter_enc.c',
		'external/krglhwebp/libwebp/src/enc/frame_enc.c',
		'external/krglhwebp/libwebp/src/enc/histogram_enc.c',
		'external/krglhwebp/libwebp/src/enc/iterator_enc.c',
		'external/krglhwebp/libwebp/src/enc/near_lossless_enc.c',
		'external/krglhwebp/libwebp/src/enc/picture_csp_enc.c',
		'external/krglhwebp/libwebp/src/enc/picture_enc.c',
		'external/krglhwebp/libwebp/src/enc/picture_psnr_enc.c',
		'external/krglhwebp/libwebp/src/enc/picture_rescale_enc.c',
		'external/krglhwebp/libwebp/src/enc/picture_tools_enc.c',
		'external/krglhwebp/libwebp/src/enc/predictor_enc.c',
		'external/krglhwebp/libwebp/src/enc/quant_enc.c',
		'external/krglhwebp/libwebp/src/enc/syntax_enc.c',
		'external/krglhwebp/libwebp/src/enc/token_enc.c',
		'external/krglhwebp/libwebp/src/enc/tree_enc.c',
		'external/krglhwebp/libwebp/src/enc/vp8l_enc.c',
		'external/krglhwebp/libwebp/src/enc/webp_enc.c',
		'external/krglhwebp/libwebp/src/mux/anim_encode.c',
		'external/krglhwebp/libwebp/src/mux/muxedit.c',
		'external/krglhwebp/libwebp/src/mux/muxinternal.c',
		'external/krglhwebp/libwebp/src/mux/muxread.c',
		'external/krglhwebp/libwebp/src/utils/bit_reader_utils.c',
		'external/krglhwebp/libwebp/src/utils/bit_writer_utils.c',
		'external/krglhwebp/libwebp/src/utils/color_cache_utils.c',
		'external/krglhwebp/libwebp/src/utils/filters_utils.c',
		'external/krglhwebp/libwebp/src/utils/huffman_encode_utils.c',
		'external/krglhwebp/libwebp/src/utils/huffman_utils.c',
		'external/krglhwebp/libwebp/src/utils/quant_levels_dec_utils.c',
		'external/krglhwebp/libwebp/src/utils/quant_levels_utils.c',
		'external/krglhwebp/libwebp/src/utils/random_utils.c',
		'external/krglhwebp/libwebp/src/utils/rescaler_utils.c',
		'external/krglhwebp/libwebp/src/utils/thread_utils.c',
		'external/krglhwebp/libwebp/src/utils/utils.c',
	]
	krkrsdl2_includes += [
		'external/krglhwebp/libwebp',
		'external/krglhwebp/libwebp/src',
		'external/krglhwebp/libwebp_config',
	]
endif
if get_option('builtin_plugin_enable_kri18n') == true
	krkrsdl2_src += [
		'external/kri18n/main.cpp',
		'external/kri18n/i18n/Entry.cpp',
		'external/kri18n/i18n/I18nUtils.cpp',
		'external/kri18n/i18n/MO.cpp',
		'external/kri18n/i18n/plural-exp.c',
		'external/kri18n/i18n/plural.c',
	]
endif
if get_option('builtin_plugin_enable_ksupport') == true
	krkrsdl2_src += [
		'external/ksupport/layer.cpp',
		'external/ksupport/rect.cpp',
		'external/ksupport/table.cpp',
		'external/ksupport/timeline.cpp',
		'external/ksupport/utils.cpp',
	]
endif
if get_option('builtin_plugin_enable_savestruct') == true
	krkrsdl2_src += [
		'external/saveStruct/Main.cpp',
	]
endif
if get_option('builtin_plugin_enable_varfile') == true
	krkrsdl2_src += [
		'external/varfile/Main.cpp',
	]
endif
if get_option('builtin_plugin_enable_scriptsex') == true
	krkrsdl2_src += [
		'external/scriptsEx/Main.cpp',
	]
endif
if target_machine.system() == 'emscripten'
	krkrsdl2_src += [
		'src/plugins/kremscripten.cpp',
	]
endif
krkrsdl2_cflags = []
krkrsdl2_cflags += [
	'-DTVP_OPUS_DECODER_IMPLEMENT',
	'-DTVP_LOG_TO_COMMANDLINE_CONSOLE',
	'-DHAVE_CONFIG_H',
	'-DFT2_BUILD_LIBRARY',
	'-DUSE_ALLOCA',
	'-DOPUS_BUILD',
	'-DHAVE_LRINTF',
	'-DHAVE_LRINT',
	'-DFLOAT_APPROX',
]
if target_machine.system() == 'emscripten'
	krkrsdl2_cflags += [
		'-s', 'DISABLE_EXCEPTION_CATCHING=0',
		'-s', 'EXIT_RUNTIME=1',
	]
	if get_option('emscripten_enable_wasm_simd') == true
		krkrsdl2_cflags += [
			'-msimd128',
			'-msse4.2',
		]
	endif
	if get_option('emscripten_enable_threads') == true
		krkrsdl2_cflags += [
			'-s', 'USE_PTHREADS=1',
			'-s', 'PROXY_TO_PTHREAD=1',
			'-s', 'OFFSCREEN_FRAMEBUFFER=1',
		]
	endif
endif
krkrsdl2_cppflags = []
krkrsdl2_cppflags += krkrsdl2_cflags
krkrsdl2_cppflags += ['-std=c++11']
if cxx.get_id() == 'gcc'
	krkrsdl2_cppflags += ['-fno-delete-null-pointer-checks']
endif
krkrsdl2_ldflags = []
if target_machine.system() == 'linux'
	krkrsdl2_ldflags += ['-pthread']
endif
if target_machine.system() == 'emscripten'
	krkrsdl2_ldflags += [
		'-s', 'DISABLE_EXCEPTION_CATCHING=0',
		'-s', 'EXIT_RUNTIME=1',
		'-s', 'ALLOW_MEMORY_GROWTH=1',
		'-s', 'INITIAL_MEMORY=' + get_option('emscripten_initial_memory').to_string(),
		'-s', 'FORCE_FILESYSTEM=1',
		'--bind',
		'--no-heap-copy',
	]
	if get_option('emscripten_enable_js_fallback') == true
		krkrsdl2_ldflags += [
			'-s', 'WASM=2',
		]
	else
		krkrsdl2_ldflags += [
			'-s', 'WASM=1',
		]
	endif
	if get_option('emscripten_enable_threads') == true
		krkrsdl2_ldflags += [
			'-s', 'USE_PTHREADS=1',
			'-s', 'PROXY_TO_PTHREAD=1',
			'-s', 'OFFSCREEN_FRAMEBUFFER=1',
		]
	endif
	if get_option('emscripten_enable_source_maps') == true
		krkrsdl2_ldflags += [
			'-g4',
			'--source-map-base', 'http://127.0.0.1:8080/'
		]
	endif
endif
krkrsdl2_dependson = [
	sdl2_dep,
]

if get_option('enable_coretext_rasterizer') == true
	if target_machine.system() == 'darwin'
		coretext_dep = dependency('appleframeworks', modules: 'CoreText')
		krkrsdl2_dependson += [
			coretext_dep
		]
		krkrsdl2_cppflags += [
			'-DKRKRZ_CORETEXT_SUPPORT',
			'-mmacosx-version-min=10.15',
		]
		krkrsdl2_src += [
			'external/krkrz/visual/darwin/CoreTextFontFallback.mm',
			'external/krkrz/visual/darwin/CoreTextFontRasterizer.cc',
			'external/krkrz/visual/darwin/CoreTextFontRasterizer.h',
			'external/krkrz/visual/darwin/CoreTextFontStorage.cc',
			'external/krkrz/visual/darwin/CoreTextFontStorage.h',
		]
	endif
endif

if get_option('builtin_plugin_enable_menu') == true
	krkrsdl2_src += [
		'src/plugins/menu/menu.cc',
	]
	if target_machine.system() == 'darwin'
		cocoa_dep = dependency('appleframeworks', modules: 'Cocoa')
		krkrsdl2_dependson += [
			cocoa_dep
		]
		krkrsdl2_src += [
			'src/plugins/menu/cocoa.mm',
		]
	endif
endif

if get_option('enable_wavemixer_openal') == true
	krkrsdl2_cppflags += [
		'-DTVP_USE_OPENAL',
	]
	if target_machine.system() == 'darwin'
		openal_dep = dependency('appleframeworks', modules: 'OpenAL')
		krkrsdl2_dependson += [
			openal_dep
		]
	else
		openal_dep = dependency('openal', method: 'pkg-config')
		krkrsdl2_dependson += [
			openal_dep
		]
	endif
endif

krkrsdl2_override_options = []
if target_machine.system() == 'emscripten'
	krkrsdl2_override_options += [
		'optimization=s',
		'debug=false',
		'b_lto=true'
	]
endif
krkrsdl2 = executable(
	'krkrsdl2',
	sources: krkrsdl2_src,
	c_args: krkrsdl2_cflags,
	cpp_args: krkrsdl2_cppflags,
	objcpp_args: krkrsdl2_cppflags,
	link_args: krkrsdl2_ldflags,
	include_directories: include_directories(krkrsdl2_includes),
	dependencies: krkrsdl2_dependson,
	install: true,
	override_options: krkrsdl2_override_options
)
if target_machine.system() == 'emscripten'
	krkrsdl2_data_output_basename = 'data'
	krkrsdl2_input_folder_path = '' + get_option('krkrsdl2_data_path')
	if krkrsdl2_input_folder_path == ''
		krkrsdl2_input_folder_path = join_paths(meson.source_root(), 'data')
	endif
	krkrsdl2_data = custom_target(krkrsdl2_data_output_basename,
		output: [krkrsdl2_data_output_basename + '.data', krkrsdl2_data_output_basename + '.js'],
		depend_files: krkrsdl2_input_folder_path,
		command: [
			'bash',
			'-c',
			'python3 $EM_CACHE/../tools/file_packager.py $@',
			'', 
			krkrsdl2_data_output_basename + '.data',
			'--preload', krkrsdl2_input_folder_path + '@/',
			'--js-output=' + krkrsdl2_data_output_basename + '.js'
		],
		build_by_default: true
	)
	krkrsdl2_dependson += krkrsdl2_data
	conf_data = configuration_data()
	conf_data.set('exe_name', krkrsdl2.name())
	conf_data.set('data_name', krkrsdl2_data_output_basename)
	krkrsdl2_html = configure_file(input: 'src/config/index.html.in',
		output: 'index.html',
		configuration: conf_data
	)
	krkrsdl2_dependson += krkrsdl2_html
endif
