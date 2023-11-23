import platform
import psutil
import cpuinfo
import subprocess
import datetime
import time

# Basic system and CPU information
info = {
    "Operating System": platform.system(),
    "OS Version": platform.release(),
    "Processor Name": platform.processor(),
    "Physical Cores": psutil.cpu_count(logical=False),
    "Total Cores": psutil.cpu_count(logical=True),
    "CPU Frequency": f"{psutil.cpu_freq().current:.2f}Mhz",
    "Total RAM": f"{psutil.virtual_memory().total / (1024 ** 3):.2f} GB"
}

# Detailed processor information
cpu_info = cpuinfo.get_cpu_info()
extended_info = {
    "CPU Architecture": cpu_info['arch'],
    "Processor Brand": cpu_info['brand_raw'],
    "Threads per Core": cpu_info.get('threads_per_core'),
    "L2 Cache (per core)": f"{cpu_info.get('l2_cache_size', 0) / 1024} KB",
    "L3 Cache (total)": f"{cpu_info.get('l3_cache_size', 0) / 1024} KB",
    "Bus Size": f"{cpu_info.get('bits', 0)} bits",
    "Processor Family": cpu_info['family']
}

# Additional hardware information using system commands
def run_command(command):
    return subprocess.check_output(command, shell=True, text=True).strip()

additional_info = {
    "Graphics Card": run_command("wmic path win32_videocontroller get name"),
    "Battery Info": run_command("wmic path win32_battery get estimatedchargeremaining"),
    "System Manufacturer": run_command("wmic computersystem get manufacturer"),
    "System Model": run_command("wmic computersystem get model"),
    "Disk Storage": f"Total: {psutil.disk_usage('/').total / (1024 ** 3):.2f} GB, Free: {psutil.disk_usage('/').free / (1024 ** 3):.2f} GB",
    "System Uptime": str(datetime.timedelta(seconds=int(time.time() - psutil.boot_time())))
}

# Network interfaces information
network_info = {}
for interface_name, interface_addresses in psutil.net_if_addrs().items():
    network_info[interface_name] = {}
    for address in interface_addresses:
        if str(address.family) == 'AddressFamily.AF_INET':
            network_info[interface_name]['IP Address'] = address.address

# Print information
for key, value in info.items():
    print(f"{key}: {value}")

for key, value in extended_info.items():
    print(f"{key}: {value}")

for key, value in additional_info.items():
    print(f"{key}: {value}")

for interface, addresses in network_info.items():
    print(f"{interface}: {addresses}")
