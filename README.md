# Copy With System Rights

Copies/Deletes/renames files that are permission locked by the system.
This functionality is done by using a windows service that runs with system rights to copy/delete/rename files that are locked by the system.

The service is embedded the in main app and is extracted and registered on running the app, and unregistered and deleted on closing the app.

The app works in GUI mode as well as command line mode.
For GUI mode, run the app without any arguments.
For command line mode, run the app with the following arguments:
```
Usage: CopyWSR.exe[/copy|/delete|/rename][source][destination]
```
The source and destination paths should be absolute paths.

The App is built using MFC/C++.


