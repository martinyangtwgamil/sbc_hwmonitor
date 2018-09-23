# sbchwmonitor

Join work flow:

	SBC-1
	SBC-2	--SSH---	back-end controller SSHD	=== 	authentication by configuration
	SBC-3


Heart beat work flow:

	SBC-1
	SBC-2	--SSH---	back-end controller SSHD	=== 	authenticated	===>	receiving monitor data
	SBC-3


Passive upgrade patch flow:

	SBC-1												upgrade process-1
	SBC-2	--SSH---	back-end controller SSHD	=== 	heart beat service	---	upgrade process-2
	SBC-3												upgrade process-3

	1.Simulated SBC with client.sh, heart_beat.sh, hb
	2.Simulated backend controller with hb_service

Security:

	ssh/sshd is responsible for security requirement, it isolated the requirement from the heartbeat service.
	Assume all SBCs are assigned one pair of unique pub./pri. key during manufacture procedure.
	All public key of upcoming SBCs is registered before the deployment, the back-end controller have been input for valid key list.
	Backend controller could identify which SBC is connecting without password.

	refer to "sshd_config_sample" for the configuration

	Reference:
	https://www.cyberciti.biz/tips/linux-unix-bsd-openssh-server-best-practices.html
	https://www.digitalocean.com/community/tutorials/ssh-essentials-working-with-ssh-servers-clients-and-keys


Route External Traffic to Internal IP with NAT:
	1. Route authenticated external traffic to an internal ip, in case the seesion has been traced.
	2. Backend service listen to certain internal only IP/port, sessions are routed to such IP/port.

	Reference:
	ftp://ftp.wayne.edu/ldp/en/linux-ip/ch05s03.html


Heart Beat Service at internal IP:

	HBS is implemented with c, which create threaded for each heartbeat.
	Once heartbeat received, the SBCs sent data to controller.

	Reference:
	https://www.binarytides.com/socket-programming-c-linux-tutorial/
	https://gist.github.com/Alexey-N-Chernyshov/4634731

//TODO: Controller store data into database.

Passive Upgrade:

	If there is any need to upgrade any firmware or patch to SBCs, the SBC takes the file token after the heart beat authenticated.
	No patch is pushed by the backend service since SBCs is required to connect to controller periodically.
	SBC should implement power failure detection and power governor to save power consumption, or flash layout mechanism like dual bank to ensure no servie would be stopped for the sake of power failure during upgrading.
	

Heart Beat Interval:

	Since the system requirment is based on
	1. Solar planet, the power consumption is critical.
	2. ADC takes energy during detecting the current and voltage.
	3. Wireless communication takes much enery to transmit packets
	4. Sunlight would not change dramtically in the manner of second

	I refer 60 seconds would satisfy the requirement without much impact.

	Per second monitor on SBCs:
	1. Detect current, voltage, or irradiance, and accumulated the data per second.

	Per minute trasnmission to controller:
	1. Deliever file to backend service controller per minute, as heart beat.

SBC Board Survey:

	The beagle board, Raspberry Pi are way too "heavy" for specified usage.
	I would suggest MCUs-based solution, add-on LORA module to minimize the power consumption for the SBC.
	Refer to following link for low power stm32L series and embeded LORA
	MBED, handy OS for first developing.
	https://www.st.com/en/evaluation-tools/b-l072z-lrwan1.html
