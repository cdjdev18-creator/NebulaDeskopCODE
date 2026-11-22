const state = {
  leftW: 280,
  rightW: 350,
  centerTop: 400,
  centerBottom: 160,
  yamaVisible: true,
  dragging: null,
  startX: 0,
  startY: 0,
  initialLeftW: 0,
  initialRightW: 0,
  initialCenterTop: 0,
  initialCenterBottom: 0
};

const els = {
  leftColumn: document.getElementById('yama-panel').parentElement,
  yamaPanel: document.getElementById('yama-panel'),
  projectsPanel: document.getElementById('projects-panel'),
  mainResizer: document.getElementById('main-resizer'),
  rightResizer: document.getElementById('right-resizer'),
  yamaProjResizer: document.getElementById('yama-projects-resizer'),
  editorTermResizer: document.getElementById('editor-terminal-resizer'),
  editorPanel: document.getElementById('editor-panel'),
  terminalPanel: document.getElementById('terminal-panel'),
  centerColumn: document.getElementById('editor-panel').parentElement,
  rightColumn: document.getElementById('ai-panel').parentElement,
  yamaTree: document.getElementById('yama-tree'),
  projectsTree: document.getElementById('projects-tree'),
  editorContent: document.getElementById('editor-content'),
  aiInput: document.getElementById('ai-input'),
  aiSend: document.getElementById('ai-send'),
  aiConversation: document.getElementById('ai-conversation')
};

function postConfig() {
  const config = JSON.stringify({
    leftW: state.leftW,
    rightW: state.rightW,
    termH: state.centerBottom,
    mainW: Math.max(300, window.innerWidth - state.leftW - state.rightW - 8),
    yamaVisible: state.yamaVisible ? 1 : 0,
    centerTop: state.centerTop
  });
  if (window.chrome && window.chrome.webview) {
    window.chrome.webview.postMessage('SAVE_CONFIG ' + config);
  } else if (window.webview) {
    window.webview.postMessage('SAVE_CONFIG ' + config);
  }
}

function updateLayout() {
  const container = document.querySelector('.app-container');
  const cw = container.clientWidth;
  const ch = container.clientHeight;

  const minLeft = 200;
  const minRight = 250;
  const minCenter = 300;

  let leftW = Math.max(minLeft, Math.min(state.leftW, cw - minRight - minCenter - 8));
  let rightW = Math.max(minRight, Math.min(state.rightW, cw - leftW - minCenter - 8));
  let centerW = cw - leftW - rightW - 8;
  centerW = Math.max(minCenter, centerW);

  els.leftColumn.style.width = leftW + 'px';
  els.rightColumn.style.width = rightW + 'px';
  els.centerColumn.style.flex = `0 0 ${centerW}px`;
  els.centerColumn.style.width = centerW + 'px';

  state.leftW = leftW;
  state.rightW = rightW;

  const editorPanel = els.editorPanel;
  const terminalPanel = els.terminalPanel;
  const centerH = ch;
  let centerTop = Math.max(120, Math.min(state.centerTop, centerH - state.centerBottom - 20));
  let centerBottom = Math.max(80, Math.min(state.centerBottom, centerH - centerTop - 20));

  editorPanel.style.flex = `0 0 ${centerTop}px`;
  editorPanel.style.height = centerTop + 'px';
  terminalPanel.style.flex = `0 0 ${centerBottom}px`;
  terminalPanel.style.height = centerBottom + 'px';

  state.centerTop = centerTop;
  state.centerBottom = centerBottom;

  if (state.yamaVisible) {
    els.yamaPanel.style.display = 'flex';
    els.projectsPanel.style.display = 'flex';
    els.yamaProjResizer.style.display = 'block';
  } else {
    els.yamaPanel.style.display = 'none';
    els.projectsPanel.style.display = 'none';
    els.yamaProjResizer.style.display = 'none';
  }

  postConfig();
}

function startDrag(kind, e) {
  state.dragging = kind;
  state.startX = e.clientX;
  state.startY = e.clientY;
  state.initialLeftW = state.leftW;
  state.initialRightW = state.rightW;
  state.initialCenterTop = state.centerTop;
  state.initialCenterBottom = state.centerBottom;
  document.body.style.cursor = kind === 'left' || kind === 'right' ? 'ew-resize' : 'row-resize';
  document.addEventListener('mousemove', onDrag);
  document.addEventListener('mouseup', endDrag);
}

function onDrag(e) {
  const dx = e.clientX - state.startX;
  const dy = e.clientY - state.startY;

  if (state.dragging === 'left') {
    state.leftW = state.initialLeftW + dx;
  } else if (state.dragging === 'right') {
    state.rightW = state.initialRightW - dx;
  } else if (state.dragging === 'yamaProj') {
    state.centerTop = state.initialCenterTop + dy;
    state.centerBottom = state.initialCenterBottom - dy;
  } else if (state.dragging === 'editorTerm') {
    state.centerTop = state.initialCenterTop + dy;
    state.centerBottom = state.initialCenterBottom - dy;
  }

  updateLayout();
}

function endDrag() {
  state.dragging = null;
  document.body.style.cursor = '';
  document.removeEventListener('mousemove', onDrag);
  document.removeEventListener('mouseup', endDrag);
}

els.mainResizer.addEventListener('mousedown', (e) => startDrag('left', e));
els.rightResizer.addEventListener('mousedown', (e) => startDrag('right', e));
els.yamaProjResizer.addEventListener('mousedown', (e) => startDrag('yamaProj', e));
els.editorTermResizer.addEventListener('mousedown', (e) => startDrag('editorTerm', e));

window.addEventListener('resize', () => updateLayout());
window.addEventListener('load', () => updateLayout());

function toggleTreeItem(item) {
  const children = item.querySelector('.tree-children');
  const toggle = item.querySelector('.tree-toggle');
  const expanded = item.classList.contains('expanded');
  if (children) {
    if (expanded) {
      item.classList.remove('expanded');
      item.classList.add('collapsed');
      toggle.textContent = '▶';
      children.style.display = 'none';
    } else {
      item.classList.remove('collapsed');
      item.classList.add('expanded');
      toggle.textContent = '▼';
      children.style.display = 'block';
    }
  }
}

function selectFile(path) {
  document.querySelectorAll('.tree-item.selected').forEach(el => el.classList.remove('selected'));
  const item = document.querySelector(`[data-path="${path}"]`);
  if (item) item.classList.add('selected');

  const content = els.editorContent;
  if (path === 'src/main.cpp') {
    content.innerHTML = `<div style="color:#d4d4d4;font-family:Consolas,monospace;font-size:14px;line-height:1.6;">#include &lt;windows.h&gt;<br><br>LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {<br>    switch (uMsg) {<br>        case WM_DESTROY:<br>            PostQuitMessage(0);<br>            return 0;<br>        default:<br>            return DefWindowProcW(hwnd, uMsg, wParam, lParam);<br>    }<br>}<br><br>int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {<br>    return 0;<br>}</div>`;
  } else if (path === 'build.bat') {
    content.innerHTML = `<div style="color:#d4d4d4;font-family:Consolas,monospace;font-size:14px;line-height:1.6;">@echo off<br>setlocal<br><br>if not exist "EXE" mkdir "EXE"<br><br>g++ -DUNICODE -D_UNICODE src\\main.cpp -o EXE\\NebulaDeskopCODE.exe -mwindows -luser32 -lgdi32 -lcomctl32 -lstdc++ -lshlwapi -lole32 -loleaut32 -llibWebView2Loader<br><br>if errorlevel 1 (<br>    echo Error de compilacion.<br>    exit /b 1<br>)<br><br>echo Compilacion exitosa: EXE\\NebulaDeskopCODE.exe<br>pause</div>`;
  } else {
    content.innerHTML = `<div style="color:#888;font-style:italic;">Vista previa de: ${path}<br><br>Selecciona un archivo de código para editar.</div>`;
  }
}

els.yamaTree.addEventListener('click', (e) => {
  const item = e.target.closest('.tree-item');
  if (!item) return;
  const toggle = item.querySelector('.tree-toggle');
  if (e.target === toggle) {
    toggleTreeItem(item);
    return;
  }
  if (item.classList.contains('file')) {
    selectFile(item.getAttribute('data-path'));
  } else {
    toggleTreeItem(item);
  }
});

els.projectsTree.addEventListener('click', (e) => {
  const item = e.target.closest('.tree-item');
  if (!item) return;
  const toggle = item.querySelector('.tree-toggle');
  if (e.target === toggle) {
    toggleTreeItem(item);
    return;
  }
  selectFile(item.getAttribute('data-path'));
});

function addAiMessage(text, type) {
  const div = document.createElement('div');
  div.className = 'ai-message ' + type;
  div.textContent = text;
  els.aiConversation.appendChild(div);
  els.aiConversation.scrollTop = els.aiConversation.scrollHeight;
}

els.aiSend.addEventListener('click', () => {
  const text = els.aiInput.value.trim();
  if (!text) return;
  addAiMessage(text, 'user');
  els.aiInput.value = '';
  setTimeout(() => addAiMessage('Mensaje recibido. (IA no conectada en esta versión)', 'agent'), 500);
});

els.aiInput.addEventListener('keydown', (e) => {
  if (e.key === 'Enter') {
    els.aiSend.click();
  }
});

setTimeout(() => {
  addAiMessage('Nebula Agent inicializado. Conversaciones aparecerán aquí.', 'system');
}, 500);
