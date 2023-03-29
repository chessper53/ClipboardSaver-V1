# ClipboardSaver-V1
ClipboardSaver is an add-on to the Clipboard history that Windows gives you by default. It improves it so that all clipboard entries are saved persistently, meaning that the history is still intact even after you reboot your PC. 
## How it Works
All clipboard entries are being saved in a JSON file called `pastEntries.json`. The code then adds all the objects from said file into your history, allowing you to access them like normal. Once the program has been started once, it is added to the Autostart and will function automatically.
