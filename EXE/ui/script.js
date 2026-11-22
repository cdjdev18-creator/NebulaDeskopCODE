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
  initialCenterBottom: 0,
  tabs: [],
  activeTabId: null,
  currentProject: null,
  dirtyFiles: new Set()
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
  tabBar: null,
  editorWrap: null,
  editorTextarea: null,
  editorHighlight: null,
  editorLineNumbers: null,
  terminalContent: document.getElementById('terminal-content'),
  aiInput: document.getElementById('ai-input'),
  aiSend: document.getElementById('ai-send'),
  aiConversation: document.getElementById('ai-conversation'),
  btnConfig: document.querySelector('.btn-config'),
  btnAgentes: document.querySelector('.btn-agentes'),
  btnAdd: document.querySelector('.btn-add')
};

function postMessage(msg) {
  if (window.chrome && window.chrome.webview) {
    window.chrome.webview.postMessage(msg);
  } else if (window.webview) {
    window.webview.postMessage(msg);
  }
}

function postConfig() {
  const config = JSON.stringify({
    leftW: state.leftW,
    rightW: state.rightW,
    termH: state.centerBottom,
    mainW: Math.max(300, window.innerWidth - state.leftW - state.rightW - 8),
    yamaVisible: state.yamaVisible ? 1 : 0,
    centerTop: state.centerTop
  });
  postMessage('SAVE_CONFIG ' + config);
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
window.addEventListener('load', () => {
  initEditorDOM();
  updateLayout();
  loadProjectsFromCpp();
});

function initEditorDOM() {
  const editorPanel = els.editorPanel;
  const header = editorPanel.querySelector('.panel-header');
  const content = editorPanel.querySelector('.panel-content');

  els.tabBar = document.createElement('div');
  els.tabBar.className = 'tab-bar';
  els.tabBar.innerHTML = '<div class="tab-tab new-tab" id="tab-new">+</div>';
  content.insertBefore(els.tabBar, content.firstChild);

  els.editorWrap = document.createElement('div');
  els.editorWrap.className = 'editor-wrap';

  els.editorLineNumbers = document.createElement('div');
  els.editorLineNumbers.className = 'editor-line-numbers';
  els.editorLineNumbers.textContent = '1';

  els.editorHighlight = document.createElement('div');
  els.editorHighlight.className = 'editor-highlight';
  els.editorHighlight.textContent = '';

  els.editorTextarea = document.createElement('textarea');
  els.editorTextarea.className = 'editor-textarea';
  els.editorTextarea.spellcheck = false;
  els.editorTextarea.wrap = 'off';
  els.editorTextarea.placeholder = 'Selecciona un archivo desde Yama para editar...';
  els.editorTextarea.readOnly = true;

  els.editorWrap.appendChild(els.editorLineNumbers);
  els.editorWrap.appendChild(els.editorHighlight);
  els.editorWrap.appendChild(els.editorTextarea);

  const oldContent = content.querySelector('.editor-placeholder');
  if (oldContent) oldContent.remove();
  content.appendChild(els.editorWrap);

  syncScroll();
  els.editorTextarea.addEventListener('scroll', syncScroll);
  els.editorTextarea.addEventListener('input', onEditorInput);
  els.editorTextarea.addEventListener('keydown', onEditorKeydown);
}

function syncScroll() {
  const scrollTop = els.editorTextarea.scrollTop;
  const scrollLeft = els.editorTextarea.scrollLeft;
  els.editorHighlight.scrollTop = scrollTop;
  els.editorHighlight.scrollLeft = scrollLeft;
  els.editorLineNumbers.scrollTop = scrollTop;
}

function getLineNumbers(count) {
  let lines = '';
  for (let i = 1; i <= count; i++) lines += i + '\n';
  return lines;
}

function onEditorInput() {
  if (!state.activeTabId) return;
  const tab = state.tabs.find(t => t.id === state.activeTabId);
  if (!tab) return;
  tab.content = els.editorTextarea.value;
  state.dirtyFiles.add(tab.id);
  updateTabDirtyIndicator(tab.id);
  highlightCode(tab.content, tab.language);
  updateLineNumbers();
}

function onEditorKeydown(e) {
  if (e.ctrlKey && e.key === 's') {
    e.preventDefault();
    saveActiveFile();
  }
}

function updateLineNumbers() {
  const lines = (els.editorTextarea.value.match(/\n/g) || []).length + 1;
  els.editorLineNumbers.textContent = getLineNumbers(lines);
}

function highlightCode(code, language) {
  if (!code) {
    els.editorHighlight.textContent = '';
    return;
  }
  els.editorHighlight.innerHTML = highlight(code, language);
}

function escapeHtml(text) {
  const div = document.createElement('div');
  div.textContent = text;
  return div.innerHTML;
}

function highlight(code, language) {
  const lang = language || 'text';
  const html = escapeHtml(code);

  const rules = getHighlightRules(lang);
  let result = html;

  for (const rule of rules) {
    const regex = rule.regex;
    const replacement = rule.replacement;
    result = result.replace(regex, replacement);
  }

  return result;
}

function getHighlightRules(language) {
  const common = [
    { regex: /&amp;gt;(&amp;#39;|&amp;quot;|&amp;lt;|&amp;gt;|&amp;amp;|&#39;|&#34;|&#60;|&#62;|&#38;)/g, replacement: '$1' }
  ];

  const byLang = {
    c: [
      { regex: /\/\/[^\n]*/g, replacement: '<span class="hl-comment">$&</span>' },
      { regex: /\/\*[\s\S]*?\*\//g, replacement: '<span class="hl-comment">$&</span>' },
      { regex: /"[^"]*"/g, replacement: '<span class="hl-string">$&</span>' },
      { regex: /'[^']*'/g, replacement: '<span class="hl-string">$&</span>' },
      { regex: /\b(int|char|void|float|double|struct|union|enum|const|static|extern|register|volatile|unsigned|signed|short|long|return|if|else|while|for|do|switch|case|break|continue|goto|sizeof|typedef|void)\b/g, replacement: '<span class="hl-keyword">$&</span>' },
      { regex: /\b#include\s+[<"][^>"]+[>"]/g, replacement: '<span class="hl-preprocessor">$&</span>' },
      { regex: /#\w+/g, replacement: '<span class="hl-preprocessor">$&</span>' }
    ],
    cpp: [],
    'c++': [],
    'c#': [],
    java: [],
    python: [
      { regex: /#[^\n]*/g, replacement: '<span class="hl-comment">$&</span>' },
      { regex: /"""[\s\S]*?"""/g, replacement: '<span class="hl-string">$&</span>' },
      { regex: /'[^']*'/g, replacement: '<span class="hl-string">$&</span>' },
      { regex: /"[^"]*"/g, replacement: '<span class="hl-string">$&</span>' },
      { regex: /\b(def|class|return|if|else|elif|while|for|in|import|from|as|with|try|except|finally|raise|yield|lambda|None|True|False|and|or|not|is|async|await|global|nonlocal|del|pass|break|continue)\b/g, replacement: '<span class="hl-keyword">$&</span>' },
      { regex: /@\w+/g, replacement: '<span class="hl-decorator">$&</span>' }
    ],
    javascript: [
      { regex: /\/\/[^\n]*/g, replacement: '<span class="hl-comment">$&</span>' },
      { regex: /\/\*[\s\S]*?\*\//g, replacement: '<span class="hl-comment">$&</span>' },
      { regex: /`[^`]*`/g, replacement: '<span class="hl-string">$&</span>' },
      { regex: /"[^"]*"/g, replacement: '<span class="hl-string">$&</span>' },
      { regex: /'[^']*'/g, replacement: '<span class="hl-string">$&</span>' },
      { regex: /\b(let|const|var|function|return|if|else|while|for|in|of|import|export|from|as|with|try|catch|finally|throw|async|await|class|extends|new|this|typeof|instanceof|void|delete|switch|case|break|continue|default|null|true|false|undefined)\b/g, replacement: '<span class="hl-keyword">$&</span>' }
    ],
    typescript: [],
    html: [
      { regex: /<!--[\s\S]*?-->/g, replacement: '<span class="hl-comment">$&</span>' },
      { regex: /<[^>]+>/g, replacement: '<span class="hl-tag">$&</span>' },
      { regex: /="[^"]*"/g, replacement: '=<span class="hl-attr-value">$&</span>' }
    ],
    css: [
    { regex: /\/\*[\s\S]*?\*\//g, replacement: '<span class="hl-comment">$&</span>' },
    { regex: /[.#][\w-]+/g, replacement: '<span class="hl-selector">$&</span>' },
    { regex: /[\w-]+\s*:/g, replacement: '<span class="hl-property">$&</span>' },
    { regex: /:[^;}]+;/g, replacement: '<span class="hl-value">$&</span>' },
    { regex: /#[0-9a-fA-F]{3,6}/g, replacement: '<span class="hl-hex">$&</span>' }
    ],
    json: [
      { regex: /"([^"]*)"\s*:/g, replacement: '<span class="hl-json-key">"$1"</span>:' },
      { regex: /:"([^"]*)"/g, replacement: ':"<span class="hl-json-string">$1</span>"' },
      { regex: /\b(true|false|null)\b/g, replacement: '<span class="hl-json-literal">$&</span>' },
      { regex: /\b\d+\.?\d*\b/g, replacement: '<span class="hl-json-number">$&</span>' }
    ],
    xml: [
      { regex: /<!--[\s\S]*?-->/g, replacement: '<span class="hl-comment">$&</span>' },
      { regex: /<[^>]+>/g, replacement: '<span class="hl-tag">$&</span>' },
      { regex: /="[^"]*"/g, replacement: '=<span class="hl-attr-value">$&</span>' }
    ],
    text: []
  };

  const langRules = byLang[language] || byLang.text;
  return [...common, ...langRules];
}

function getLanguageFromPath(path) {
  const ext = path.split('.').pop().toLowerCase();
  const map = {
    c: 'c', h: 'c',
    cpp: 'cpp', cc: 'cpp', cxx: 'cpp', hpp: 'cpp', hxx: 'cpp',
    cs: 'c#',
    java: 'java',
    py: 'python',
    js: 'javascript',
    ts: 'typescript',
    html: 'html',
    htm: 'html',
    css: 'css',
    json: 'json',
    xml: 'xml',
    txt: 'text',
    md: 'text',
    ini: 'text',
    log: 'text',
    bat: 'text',
    sh: 'text'
  };
  return map[ext] || 'text';
}

function getFileName(path) {
  const parts = path.split(/[\\/]/);
  return parts[parts.length - 1] || path;
}

function openFile(path) {
  if (!path) return;

  let tab = state.tabs.find(t => t.path === path);
  if (!tab) {
    tab = {
      id: 'tab-' + Date.now(),
      path: path,
      name: getFileName(path),
      language: getLanguageFromPath(path),
      content: '',
      dirty: false
    };
    state.tabs.push(tab);
    renderTabBar();
  }

  setActiveTab(tab.id);

  postMessage('READ_FILE|' + path);
}

function setActiveTab(tabId) {
  state.activeTabId = tabId;
  const tab = state.tabs.find(t => t.id === tabId);
  if (!tab) return;

  els.editorTextarea.value = tab.content;
  els.editorTextarea.readOnly = false;
  els.editorTextarea.placeholder = '';
  highlightCode(tab.content, tab.language);
  updateLineNumbers();
  renderTabBar();
  updateEditorHeader(tab.name);
}

function updateEditorHeader(name) {
  const header = els.editorPanel.querySelector('.panel-header');
  if (header) header.textContent = 'Editor - ' + name;
}

function renderTabBar() {
  if (!els.tabBar) return;
  els.tabBar.innerHTML = '';

  const newTab = document.createElement('div');
  newTab.className = 'tab-tab new-tab';
  newTab.textContent = '+';
  newTab.title = 'Nuevo archivo';
  els.tabBar.appendChild(newTab);

  for (const tab of state.tabs) {
    const el = document.createElement('div');
    el.className = 'tab-tab' + (tab.id === state.activeTabId ? ' active' : '') + (state.dirtyFiles.has(tab.id) ? ' dirty' : '');
    el.id = 'tab-el-' + tab.id;
    el.title = tab.path;

    const nameSpan = document.createElement('span');
    nameSpan.className = 'tab-name';
    nameSpan.textContent = tab.name;
    el.appendChild(nameSpan);

    const closeBtn = document.createElement('span');
    closeBtn.className = 'tab-close';
    closeBtn.textContent = '×';
    closeBtn.onclick = (e) => { e.stopPropagation(); closeTab(tab.id); };
    el.appendChild(closeBtn);

    el.onclick = () => setActiveTab(tab.id);
    els.tabBar.appendChild(el);
  }
}

function updateTabDirtyIndicator(tabId) {
  const el = document.getElementById('tab-el-' + tabId);
  if (!el) return;
  if (state.dirtyFiles.has(tabId)) {
    el.classList.add('dirty');
  } else {
    el.classList.remove('dirty');
  }
}

function closeTab(tabId) {
  if (state.dirtyFiles.has(tabId)) {
    if (!confirm('El archivo tiene cambios no guardados. ¿Cerrar de todas formas?')) return;
  }
  state.tabs = state.tabs.filter(t => t.id !== tabId);
  state.dirtyFiles.delete(tabId);
  if (state.activeTabId === tabId) {
    state.activeTabId = state.tabs.length > 0 ? state.tabs[0].id : null;
    if (state.activeTabId) {
      setActiveTab(state.activeTabId);
    } else {
      els.editorTextarea.value = '';
      els.editorTextarea.readOnly = true;
      els.editorTextarea.placeholder = 'Selecciona un archivo desde Yama para editar...';
      els.editorHighlight.textContent = '';
      els.editorLineNumbers.textContent = '1';
      updateEditorHeader('Editor');
    }
  }
  renderTabBar();
}

function saveActiveFile() {
  if (!state.activeTabId) return;
  const tab = state.tabs.find(t => t.id === state.activeTabId);
  if (!tab) return;

  const encoded = btoa(unescape(encodeURIComponent(tab.content)));
  postMessage('WRITE_FILE|' + tab.path + '|' + encoded);
}

function loadProjectsFromCpp() {
  postMessage('LIST_PROJECTS');
}

function loadProjectStructure(projectPath) {
  state.currentProject = projectPath;
  postMessage('SET_CURRENT_PROJECT|' + projectPath);
  postMessage('READ_DIR|' + projectPath);
  renderProjectsTree();
}

function renderProjectsTree() {
  const tree = els.projectsTree;
  tree.innerHTML = '';
  for (const project of state.projects || []) {
    // This will be populated by EVENT:PROJECTS_LIST
  }
}

window.addEventListener('message', (e) => {
  const msg = e.data;
  if (typeof msg !== 'string') return;

  if (msg.startsWith('EVENT:DIR_LIST|')) {
    const rest = msg.substring(15);
    const sep = rest.indexOf('|');
    if (sep === -1) return;
    const path = rest.substring(0, sep);
    const jsonStr = rest.substring(sep + 1);
    try {
      const items = JSON.parse(jsonStr);
      renderYamaTree(path, items);
    } catch (e) {}
    return;
  }
  if (msg.startsWith('EVENT:FILE_CONTENT|')) {
    const rest = msg.substring(19);
    const sep = rest.indexOf('|');
    if (sep === -1) return;
    const path = rest.substring(0, sep);
    const b64 = rest.substring(sep + 1);
    try {
      const content = decodeURIComponent(escape(atob(b64)));
      const tab = state.tabs.find(t => t.path === path);
      if (tab) {
        tab.content = content;
        if (state.activeTabId === tab.id) {
          els.editorTextarea.value = content;
          highlightCode(content, tab.language);
          updateLineNumbers();
        }
      } else {
        const newTab = {
          id: 'tab-' + Date.now(),
          path: path,
          name: getFileName(path),
          language: getLanguageFromPath(path),
          content: content,
          dirty: false
        };
        state.tabs.push(newTab);
        renderTabBar();
        setActiveTab(newTab.id);
      }
      state.dirtyFiles.delete(path);
    } catch (e) {}
    return;
  }
  if (msg.startsWith('EVENT:FILE_SAVED|')) {
    const path = msg.substring(17);
    state.dirtyFiles.delete(path);
    const tab = state.tabs.find(t => t.path === path);
    if (tab) {
      updateTabDirtyIndicator(tab.id);
    }
    return;
  }
  if (msg.startsWith('EVENT:FILE_ERROR|')) {
    const rest = msg.substring(17);
    const sep = rest.indexOf('|');
    const path = sep === -1 ? rest : rest.substring(0, sep);
    alert('Error al guardar: ' + path);
    return;
  }
  if (msg.startsWith('EVENT:FOLDER_PICKED|')) {
    const path = msg.substring(20);
    postMessage('ADD_PROJECT|' + path);
    return;
  }
  if (msg === 'EVENT:FOLDER_CANCELLED') {
    return;
  }
  if (msg.startsWith('EVENT:PROJECTS_LIST|')) {
    const jsonStr = msg.substring(20);
    try {
      state.projects = JSON.parse(jsonStr);
      renderProjectsTreeList();
    } catch (e) {}
    return;
  }
  if (msg.startsWith('EVENT:WINDOW_STATE|')) {
    return;
  }
}, false);

function renderYamaTree(basePath, items) {
  const tree = els.yamaTree;
  tree.innerHTML = '';
  for (const item of items) {
    const div = document.createElement('div');
    div.className = 'tree-item' + (item.type === 'file' ? ' file' : '');
    div.setAttribute('data-path', basePath + (basePath.endsWith('\\') || basePath.endsWith('/') ? '' : '\\') + item.name);

    if (item.type === 'dir') {
      div.innerHTML = '<span class="tree-toggle">▶</span><span class="tree-label">' + escapeHtml(item.name) + '</span>';
      div.appendChild(document.createElement('div'));
    } else {
      div.innerHTML = '<span class="tree-file">📄</span> <span class="tree-label">' + escapeHtml(item.name) + '</span>';
    }
    tree.appendChild(div);
  }
}

function renderProjectsTreeList() {
  const tree = els.projectsTree;
  tree.innerHTML = '';
  if (!state.projects || state.projects.length === 0) {
    tree.innerHTML = '<div style="color:#888;font-style:italic;padding:4px;">Sin proyectos</div>';
    return;
  }
  for (const proj of state.projects) {
    const div = document.createElement('div');
    div.className = 'tree-item';
    div.setAttribute('data-path', proj.path);
    div.innerHTML = '<span class="tree-label">📁 ' + escapeHtml(proj.name) + '</span>';
    div.onclick = (e) => {
      e.stopPropagation();
      loadProjectStructure(proj.path);
    };
    tree.appendChild(div);
  }
}

els.yamaTree.addEventListener('click', (e) => {
  const item = e.target.closest('.tree-item');
  if (!item) return;
  const toggle = item.querySelector('.tree-toggle');
  if (e.target === toggle) {
    const children = item.querySelector('.tree-children');
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
    return;
  }
  if (item.classList.contains('file')) {
    const path = item.getAttribute('data-path');
    openFile(path);
  } else {
    const path = item.getAttribute('data-path');
    const children = item.querySelector('.tree-children');
    if (children && children.children.length === 0 && !item.classList.contains('loaded')) {
      postMessage('READ_DIR|' + path);
      item.classList.add('loaded');
    }
    const expanded = item.classList.contains('expanded');
    if (!expanded) {
      item.classList.remove('collapsed');
      item.classList.add('expanded');
      toggle.textContent = '▼';
      if (children) children.style.display = 'block';
    } else {
      item.classList.remove('expanded');
      item.classList.add('collapsed');
      toggle.textContent = '▶';
      if (children) children.style.display = 'none';
    }
  }
});

els.yamaTree.addEventListener('dblclick', (e) => {
  const item = e.target.closest('.tree-item');
  if (!item) return;
  if (item.classList.contains('file')) {
    const path = item.getAttribute('data-path');
    openFile(path);
  }
});

els.projectsTree.addEventListener('click', (e) => {
  const item = e.target.closest('.tree-item');
  if (!item) return;
  const path = item.getAttribute('data-path');
  if (path) {
    loadProjectStructure(path);
  }
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

els.btnConfig.addEventListener('click', () => {
  postMessage('OPEN_SETTINGS');
});

els.btnAgentes.addEventListener('click', () => {
  postMessage('OPEN_AGENT_MANAGER');
});

els.btnAdd.addEventListener('click', () => {
  postMessage('PICK_FOLDER');
});
