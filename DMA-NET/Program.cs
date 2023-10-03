using System.Runtime.InteropServices;

namespace DMA
{
	internal class Program
	{
		public unsafe struct Complex
		{
			public double real;
			public double imag;
		}

		[DllImport("DMA-Core.dll", CallingConvention = CallingConvention.Cdecl)]
		public unsafe static extern void Test(int a, int b, Complex* c);

		static void Main(string[] args)
		{
			Console.WriteLine("Hello, World!");
			Console.WriteLine("Running in C#");

			Complex c = new Complex();

			unsafe
			{
				Test(1, 2, &c);
			}

			Console.WriteLine("real: {0}, imag: {1}", c.real, c.imag);
		}
	}
}