How to control for clock consumers on Milbeaut SoCs
---------------------------------------------------

*-= References =-*
  [1] Documentation/driver-api/clock.rst
  [2] Documentation/devicetree/bindings/clock/milbeaut-clock.txt
  [3] include/dt-bindings/clock/milbeaut,m20v-clock.h
  [4] SC2006A_LSI_Specification.pdf

*-= Description =-*
 -in Device Tree(DT)
  Add properties of "clocks" and "clock-names" to a node (that consumes clock),
  The "clocks" shall be <[clk_provider_aliase] [onecell_id]>, where:
   -[clk_provider_aliase]: Your clk provider aliase. Generally using "&clk".
   -[oncell_id]: If your clk provider uses it, shall be. Since Milbeaut SoCs
                 use, shall be selected from [3].
  The "clock-names" shall be strings refered from certain driver. The driver
  gets "struct clk" through this.
  And see [2] about other information.

 -in m20v-clock.h([3])
  Refer "name" on the file to determine clock which cosumed by certain node.
  A "name" consists of three parts "M20V_", clk_name, "_ID". A clk_name strictly
  matches one on the clock list of [4].
  Linux does not touch clocks that are not listed on [3], so they are controlled
  by another OS or a previous bootlooder.
  A "rate changeable" is a flg of capability to change frequency(rate). "yes"
  can call clk_set_rate() to change rate. "no" can not call it.

 -in driver ( that consumes clock )
  Use some APIs of clock to "enable", "disable", and "change rate".
  [1] and drivers/clk/clk.c are good references about the APIs.

*= Example =*

In a device tree

	sdio: sdio@19830000{
		compatible = "socionext,milbeaut-sdio";
		reg = <0x19830000 0x1000>;
		clocks = <&clk M20V_SDIOCLK_ID>, <&clk M20V_HCLK_SDIO_ID>;
		clock-names = "SDIO", "SDIOHCLK";
	};

In a driver for SDIO

	int probe_function(void)
	{
		device_node *np;
		struct clk *clk;

		/* ...other statements are skipped... */

		clk = of_clk_get_by_name(np, "SDIO"); // shall be matched as DT's
		clk_set_rate(clk, SDIO_FULL_PERFORM); // 800MHz
		clk_prepare(clk);
		clk_enable(clk);

		clk = of_clk_get_by_name(np, "SDIOHCLK"); // shall be matched as DT's
		clk_set_rate(clk, SDIOHCLK_FREQ); // actually not needed
		clk_prepare(clk);
		clk_enable(clk);

		/* ...other statements are skipped... */
	}

	int remove_function(void)
	{
		device_node *np;
		struct clk *clk;

		/* ...other statements are skipped... */

		clk = of_clk_get_by_name(np, "SDIO"); // shall be matched as DT's
		clk_disable(clk);
		clk_unprepare(clk);

		clk = of_clk_get_by_name(np, "SDIOHCLK"); // shall be matched as DT's
		clk_disable(clk);
		clk_unprepare(clk);

		/* ...other statements are skipped... */
	}

	int rate_change_function(void)
	{
		device_node *np;
		struct clk *clk;

		/* ...other statements are skipped... */

		clk = of_clk_get_by_name(np, "SDIO"); // shall be matched as DT's
		clk_disable(clk);
		clk_unprepare(clk);
		clk_set_rate(clk, SDIO_LOW_PERFORM); // certain value
		clk_prepare(clk);
		clk_enable(clk);

		/* ...other statements are skipped... */
	}
