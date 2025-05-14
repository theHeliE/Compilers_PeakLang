const { app, BrowserWindow, ipcMain, dialog } = require('electron');
const path = require('path');
const fs = require('fs');
const { execFile } = require('child_process');

function createWindow() {
  const win = new BrowserWindow({
    width: 1000,
    height: 800,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true
    }
  });

  win.loadFile('index.html');
}

app.whenReady().then(createWindow);

ipcMain.handle('select-file', async () => {
  const result = await dialog.showOpenDialog({
    properties: ['openFile']
  });
  return result.canceled ? null : result.filePaths[0];
});

function readOutputs() {
  const symbolPath = path.join(__dirname, 'SymbolTable.txt');
  const quadPath = path.join(__dirname, 'quadruples.txt');

  const symbolTable = fs.existsSync(symbolPath)
    ? fs.readFileSync(symbolPath, 'utf8')
    : '[SymbolTable.txt not found]';

  const quadruples = fs.existsSync(quadPath)
    ? fs.readFileSync(quadPath, 'utf8')
    : '[quadruples.txt not found]';

  return { symbolTable, quadruples };
}

ipcMain.handle('run-compiler', async (_, filePath) => {
  return new Promise((resolve) => {
    execFile('../compiler.exe', [filePath], (error, stdout, stderr) => {
      const output = readOutputs();
      resolve({
        stdout: error ? (stderr || error.message) : stdout,
        ...output
      });
    });
  });
});

ipcMain.handle('run-custom-text', async (_, code) => {
  const tempPath = path.join(__dirname, 'temp_input.txt');
  fs.writeFileSync(tempPath, code);

  return new Promise((resolve) => {
    execFile('../compiler.exe', [tempPath], (error, stdout, stderr) => {
      const output = readOutputs();
      resolve({
        stdout: error ? (stderr || error.message) : stdout,
        ...output
      });
    });
  });
});
