About x265
	This project is another version of x265, a open source video encoder base on the High Efficiency Video Coding (HEVC) Standard
written by liaozixin, a graduate student of Sun Yat-sen University. The target of this project is to create a realtime HEVC encoder
by open source and common contribution of world around engineers.

    There are three contributions in this project:

    First of all, by reference to the source code of x264 and HM10.0, I design a video encoder base on the HEVC standard which have
the similar design architecture and code style of x264. 

    Secondly, the encode process of HM10.0 is improved. In the encode process of HM10.0, it need to allocate the memory to store the
coding information of LCU for all LCUs in one frame of video image, this encoding method requires that a computer should have a large
number of available memory, and the memory use is extremely inefficiency. Through improving the encode process in my project, it only
allocate the memory to store the coding information of LCU for one LCU, improves the use efficiency of memory space greately. 

    Last, by use the SSE instruction of x86 assemble language, I optimize those modules which can be computed parallelly in the encoder
such as intra frame prediction, transform, quantization, inverse quantization, inverse transform, fractional pixel interpolation and
HADAMARD transform.

    Besides, I develop a relevant file content compare tool to verify the encoding result of my encoder in the develop process of
my project.

    The experimental result shows that in the case of getting the same encoding result and code stream, the project reduces the total
encoding time by 49% than HM10.0. Among them, the execute time of the extrapolation module of the intra prediction reduces by 63%,
the execute time of the Fractional-Pixel Interpolation intepolation module of the inter prediction reduces by 77%, the execute time
of the integer number transform module reduces by 44%, the execute time of the Hadamard transform module reduces by 76%! Beside, this
project also greatly improved the efficiency in the use of memory.
