How to Build
    1. Get Toolchain (arm-eabi-4.4.3) and install
       (Visit http://www.codesourcery.com/sgpp/portal/release1033 or http://android.git.kernel.org/?p=platform/prebuilt.git)   

    2. run build_kernel.sh
		
      $ export ARCH=arm
      $ export CROSS_COMPILE=~/your toolchain path/arm-eabi-4.4.3/bin/arm-eabi-
      $ ./build_kernel.sh

    3.Output Files
      -	kernel : ef51l_kernel/obj/KERNEL_OBJ/arch/arm/boot/zImage
      -	module : ef51l_kernel/obj/KERNEL_OBJ/drivers/*/*.ko
 
   
