document.getElementById('select-file').addEventListener('click', async () => {
  const filePath = await window.electronAPI.selectFile();
  if (filePath) {
    document.getElementById('file-path').innerText = filePath;
    document.getElementById('file-path').dataset.path = filePath;
  }
});

document.getElementById('run-file').addEventListener('click', async () => {
  const path = document.getElementById('file-path').dataset.path;
  if (path) {
    const result = await window.electronAPI.runCompiler(path);
    displayResults(result);
  }
});

document.getElementById('run-text').addEventListener('click', async () => {
  const code = document.getElementById('custom-input').value;
  if (code.trim()) {
    const result = await window.electronAPI.runCustomText(code);
    displayResults(result);
  }
});

function displayResults({ symbolTable, quadruples }) {
  document.getElementById('symbol').innerText = symbolTable || '[No symbol table generated]';
  document.getElementById('quad').innerText = quadruples || '[No quadruples generated]';
  showTab('symbol');
}

function showTab(tabId) {
  document.querySelectorAll('.output-tab').forEach(tab => tab.classList.add('hidden'));
  document.getElementById(tabId).classList.remove('hidden');
}
