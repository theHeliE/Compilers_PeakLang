const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electronAPI', {
  selectFile: () => ipcRenderer.invoke('select-file'),
  runCompiler: (filePath) => ipcRenderer.invoke('run-compiler', filePath),
  runCustomText: (text) => ipcRenderer.invoke('run-custom-text', text)
});
