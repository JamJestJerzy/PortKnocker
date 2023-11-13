# About
> This project is just basic cmd tool allowing to easly perform Port Knocking
# Usage
> - Install [git](https://git-scm.com/) if you haven't already
> - Open any windows terminal (`PowerShell`/`cmd`)
> - Download latest release of tool [here](https://github.com/JamJestJerzy/PortKnocker/releases)
> - Make sure you are in the same directory as tool you just downloaded
> - Run `.exe` file from your terminal:
> ```console
> PS C:\Users\JamJestJerzy> .\PortKnocker-0.1.2.0.exe
> 
>   ___                       .___                                          _____       /\
>  / _ \_/\    ________ __  __| _/____   _______  _____            ________/ ____\     / /
>  \/ \___/   /  ___/  |  \/ __ |/  _ \  \_  __ \/     \    ______ \_  __ \   __\     / /
>             \___ \|  |  / /_/ (  <_> )  |  | \/  Y Y  \  /_____/  |  | \/|  |      / /
>            /____  >____/\____ |\____/   |__|  |__|_|  /           |__|   |__|     / /
>                 \/           \/                     \/                            \/
> PortKnocking tool by Jerzy W (https://github.com/JamJestJerzy)
> address: 1 argument(s) expected. 0 provided.
> Usage: PortKnocker [--help] [--version] [--rate TIMEOUT] [--single] IP PORT_RANGE PORT
> 
> Positional arguments:
>   IP             IP (hostname) you want to kick the door of.            (e.g. 52.0.14.116)
>   PORT_RANGE     Range of ports you want to knock to.                   (e.g. 100-825)
>   PORT           Final port you want to open connection with.           (e.g. 22)
> 
> Optional arguments:
>   -h, --help     shows help message and exits
>   -v, --version  prints version information and exits
>   --rate         How many times a second send packets. [nargs=0..1] [default: 60]
>   --single       Stops after recieving response
> 
> Example usage:
>         PortKnocker-0.1.2.0.exe 10.0.43.117 8000-8100 6060
> ```
> Basically just read all of this on screen, And You'll know how to use it.
# Compilation
> - Clone this repository (```git clone https://github.com/JamJestJerzy/PortKnocker.git```)<br>
> - `cd` into cloned folder
> - Make sure you have c compiler (`g++`) installed
> - Run command ```.\build.ps1``` in Windows PowerShell
> - Compiled binary should be in ```builds``` dir with name `PortKnocker-{current version}.exe`
## Contact
> If u have any issues, questions or propositions feel free to message me on:
> - Discord: ```dash1e```
> - E-Mail: ```jerzy@j3rzy.dev```
> - Telegram: [JamJestJerzy](https://t.me/jamjestjerzy)
## License
> MIT