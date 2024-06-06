# Copy With System Rights

"Copy With System Rights" is a utility designed to manipulate files that are locked due to system permissions. It can copy, delete, or rename these files by leveraging a Windows service that operates with system rights.

## Key Features

- **System Permission Bypass**: The utility can interact with files that are locked due to system permissions. Please note that it does not bypass files locked by running applications.
- **Embedded Service**: The Windows service used by the utility is embedded within the main application. It is extracted and registered when the application runs, and unregistered and deleted when the application closes.
- **GUI and Command Line Modes**: The application can be operated in two modes. For a graphical interface, run the application without any arguments. For command line operation, specific arguments are required.

## Usage

For command line mode, use the following syntax:
```
Usage: CopyWSR.exe[/copy|/delete|/rename][source][destination]
   /copy [source][destination]
   /copy C:\Source\Dir\Filename.ext C:\Dest\Dir\Filename.ext
   /delete [source]
   /delete C:\Source\Dir\Filename.ext
   /rename [source][destination]
   /rename C:\Dir\OldFilename.ext C:\Dir\NewFilename.ext
```
Please ensure that the source and destination paths are absolute.

## Development

The application is developed using MFC/C++.


