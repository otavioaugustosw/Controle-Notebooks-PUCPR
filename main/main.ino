#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <WebServer.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "time.h"
#include <Arduino.h>

// =================================================================
// MARK: - CONTEÚDO DA INTERFACE WEB (HTML & CSS)
// =================================================================

const char* STYLE_CSS = R"rawliteral(
:root {
    --system-font: 'Inter', sans-serif;
    --background-color: #e0e5ec;
    --glass-background: rgba(255, 255, 255, 0.4);
    --glass-blur: 30px;
    --glass-border-color: rgba(255, 255, 255, 0.6);
    --text-color-primary: #ffffff;
    --text-color-secondary: rgba(251, 254, 255, 0.82);
    --accent-color-primary: #2563eb;
    --accent-color-secondary: #4f46e5;
    --success-color: #16a34a;
    --danger-color: #dc2626;
    --border-radius-large: 1.5rem;
    --border-radius-medium: 1rem;
}

body {
    font-family: var(--system-font);
    background-color: var(--background-color);
    color: var(--text-color-primary);
    overflow-x: hidden;
    margin: 0;
}

.background-wallpaper {
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: url('https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcTgbe0F3gi-OAV7xFyFrgFezK5OZXlnDOcgNg&s') no-repeat center center;
    background-size: cover;
    filter: blur(20px) saturate(1.2);
    transform: scale(1.1);
    z-index: -1;
}

.sidebar-container {
    position: fixed;
    top: 1rem;
    left: 1rem;
    bottom: 1rem;
    width: 260px;
    background: var(--glass-background);
    backdrop-filter: blur(var(--glass-blur));
    -webkit-backdrop-filter: blur(var(--glass-blur));
    border: 1px solid var(--glass-border-color);
    border-radius: var(--border-radius-large);
    display: flex;
    flex-direction: column;
    padding: 1.5rem 1rem;
    box-shadow: 0 8px 32px 0 rgba(0, 0, 0, 0.1);
    transition: all 0.3s ease;
    z-index: 1040;
}

@media (max-width: 991.98px) {
    .sidebar-container {
        display: none;
    }
}

.sidebar-header .system-title {
    font-weight: 700;
    font-size: 1.5rem;
    text-align: center;
    margin-bottom: 2rem;
    color: var(--text-color-primary);
}

.navigation-menu {
    display: flex;
    flex-direction: column;
    gap: 0.5rem;
    flex-grow: 1;
}

.nav-link-item {
    display: flex;
    align-items: center;
    padding: 0.8rem 1.2rem;
    border-radius: var(--border-radius-medium);
    text-decoration: none;
    color: var(--text-color-secondary);
    font-weight: 500;
    transition: all 0.2s ease-in-out;
}

.nav-link-item i {
    font-size: 1.2rem;
    margin-right: 1rem;
    width: 20px;
    text-align: center;
}

.nav-link-item:hover {
    background-color: rgba(0, 0, 0, 0.05);
    color: var(--text-color-primary);
}

.nav-link-item.active {
    background-image: linear-gradient(120deg, var(--accent-color-primary), var(--accent-color-secondary));
    color: white;
    font-weight: 600;
    box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
}

.sidebar-footer {
    text-align: center;
    color: var(--text-color-secondary);
    font-size: 0.8rem;
    padding-top: 1rem;
    border-top: 1px solid var(--glass-border-color);
}

.main-content {
    margin-left: 280px;
    padding: 1rem;
    transition: margin-left 0.3s ease;
}

@media (max-width: 991.98px) {
    .main-content {
        margin-left: 0;
    }
}

.main-header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 1rem;
    margin-bottom: 1rem;
}

.page-title {
    font-size: 2rem;
    font-weight: 700;
    color: var(--text-color-primary);
}

.mobile-menu-toggle {
    background: transparent;
    border: none;
    color: var(--text-color-primary);
    z-index: 1050;
}

.content-body {
    padding: 1rem;
}

.card-glass {
    background: var(--glass-background);
    backdrop-filter: blur(var(--glass-blur));
    -webkit-backdrop-filter: blur(var(--glass-blur));
    border: 1px solid var(--glass-border-color);
    border-radius: var(--border-radius-large);
    box-shadow: 0 8px 32px 0 rgba(0, 0, 0, 0.1);
    padding: 2rem;
}

.info-card {
    display: flex;
    flex-direction: column;
}

.info-card-header {
    display: flex;
    align-items: center;
    gap: 1rem;
    margin-bottom: 1rem;
}

.info-card-icon {
    font-size: 2rem;
    color: white;
}

.info-card-title {
    font-size: 1.5rem;
    font-weight: 600;
    margin: 0;
}

.info-card-body .info-card-text {
    font-size: 1rem;
    margin-bottom: 0.5rem;
}

.btn-glass-primary, .btn-glass-secondary, .btn-glass-danger {
    padding: 0.8rem 1.5rem;
    border-radius: var(--border-radius-medium);
    border: 1px solid transparent;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.2s ease;
    backdrop-filter: blur(10px);
    -webkit-backdrop-filter: blur(10px);
    white-space: nowrap;
}
.btn-glass-primary:disabled, .btn-glass-secondary:disabled, .btn-glass-danger:disabled {
    cursor: not-allowed;
    opacity: 0.6;
}

.btn-glass-primary {
    background-color: rgba(37, 99, 235, 0.8);
    color: white;
    border-color: rgba(37, 99, 235, 1);
}

.btn-glass-primary:hover:not(:disabled) {
    background-color: rgba(37, 99, 235, 1);
    box-shadow: 0 0 20px rgba(37, 99, 235, 0.5);
}

.btn-glass-secondary {
    background-color: rgba(132, 183, 253, 0.5);
    color: var(--text-color-primary);
    border-color: rgba(100, 116, 139, 0.7);
}

.btn-glass-secondary:hover:not(:disabled) {
    background-color: rgba(75, 122, 193, 0.7);
}

.btn-glass-danger {
    background-color: rgba(220, 38, 38, 0.7);
    color: white;
    border-color: rgba(220, 38, 38, 0.9);
}

.btn-glass-danger:hover:not(:disabled) {
    background-color: rgb(205, 13, 13);
}

.table-container-glass {
    background: var(--glass-background);
    backdrop-filter: blur(var(--glass-blur));
    -webkit-backdrop-filter: blur(var(--glass-blur));
    border-radius: var(--border-radius-large);
    overflow: hidden;
    padding: 1rem;
}

.table-glass {
    width: 100%;
    border-collapse: collapse;
}

.table-glass th, .table-glass td {
    padding: 1rem;
    text-align: left;
    border-bottom: 1px solid var(--glass-border-color);
    color: var(--text-color-secondary);
}

.table-glass thead th {
    font-weight: 600;
    color: var(--text-color-primary);
}

.table-glass tbody tr:last-child td {
    border-bottom: none;
}

.btn-icon-glass, .btn-icon-glass-danger {
    background: rgba(255, 255, 255, 0.3);
    border: none;
    border-radius: 50%;
    width: 36px;
    height: 36px;
    display: inline-flex;
    align-items: center;
    justify-content: center;
    color: var(--text-color-secondary);
    margin: 0 0.2rem;
    transition: all 0.2s ease;
}

.btn-icon-glass:hover:not(:disabled) {
    background: var(--accent-color-primary);
    color: white;
}

.btn-icon-glass-danger:hover:not(:disabled) {
    background: rgba(255, 108, 108, 0.6);
    color: white;
}

.badge-glass-success, .badge-glass-danger, .badge-glass-info, .badge-glass-warning {
    padding: 0.4em 0.8em;
    border-radius: 20px;
    font-weight: 600;
    font-size: 0.8rem;
}
.badge-glass-success { background-color: rgba(22, 163, 74, 0.7); color: #ffffff; }
.badge-glass-danger { background-color: rgba(220, 38, 38, 0.7); color: #ffffff; }
.badge-glass-info { background-color: rgba(37, 99, 235, 0.7); color: #ffffff; }
.badge-glass-warning { background-color: rgba(245, 158, 11, 0.7); color: #ffffff; }

.modal { z-index: 1050; }
.modal-backdrop { z-index: 1040; }
#readTagModal { z-index: 1060; }

.modal-glass {
    background: var(--glass-background);
    backdrop-filter: blur(var(--glass-blur));
    -webkit-backdrop-filter: blur(var(--glass-blur));
    border: 1px solid var(--glass-border-color);
    border-radius: var(--border-radius-large);
}

.modal-header, .modal-footer { border: none; }
.modal-title { font-weight: 600; color: var(--text-color-primary); }

.btn-close-glass {
    background: transparent url("data:image/svg+xml,%3csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16' fill='%23FFF'%3e%3cpath d='M.293.293a1 1 0 0 1 1.414 0L8 6.586 14.293.293a1 1 0 1 1 1.414 1.414L9.414 8l6.293 6.293a1 1 0 0 1-1.414 1.414L8 9.414l-6.293 6.293a1 1 0 0 1-1.414-1.414L6.586 8 .293 1.707a1 1 0 0 1 0-1.414z'/%3e%3c/svg%3e") center/1em auto no-repeat;
    border: 0;
    border-radius: 0.375rem;
    opacity: 0.8;
}
.btn-close-glass:hover { opacity: 1; }

.form-label {
    font-weight: 500;
    color: var(--text-color-secondary);
    margin-bottom: 0.5rem;
}

.input-group-glass { display: flex; gap: 0.5rem; }
.form-control-glass {
    width: 100%;
    padding: 0.8rem 1rem;
    border-radius: var(--border-radius-medium);
    border: 1px solid var(--glass-border-color);
    background-color: rgba(255, 255, 255, 0.5);
    color: var(--text-color-primary);
    outline: none;
    transition: all 0.2s ease;
}

.input-group-glass .form-control-glass { flex: 1 1 auto; }
.form-control-glass::placeholder { color: var(--text-color-secondary); }
.form-control-glass:focus {
    background-color: rgba(255, 255, 255, 0.8);
    border-color: var(--accent-color-primary);
    box-shadow: 0 0 0 3px rgba(37, 99, 235, 0.3);
}

.offcanvas-glass {
    background: var(--glass-background);
    backdrop-filter: blur(var(--glass-blur));
    -webkit-backdrop-filter: blur(var(--glass-blur));
}
.navigation-menu-mobile .nav-link-item { font-size: 1.2rem; }

.operation-icon {
    font-size: 3rem;
    color: var(--accent-color-primary);
    margin-bottom: 1rem;
    display: block;
}
.operation-title { font-weight: 700; }
.operation-subtitle { color: var(--text-color-secondary); }

#toast-container { position: fixed; bottom: 1rem; right: 1rem; z-index: 1100; }
.toast-item {
    background-color: rgba(0,0,0,0.7);
    color: white;
    padding: 1rem;
    border-radius: var(--border-radius-medium);
    margin-top: 0.5rem;
    opacity: 0;
    transition: all 0.5s ease;
    transform: translateX(100%);
}
.toast-item.show { opacity: 1; transform: translateX(0); }
.toast-item.error { background-color: rgba(220, 38, 38, 0.8); }
.toast-item.success { background-color: rgba(22, 163, 74, 0.8); }

)rawliteral";

const char* INDEX_HTML = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SGA - Início</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet">
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.3/font/bootstrap-icons.min.css">
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="/css/style.css">
</head>
<body>

<div class="background-wallpaper"></div>

<div class="sidebar-container">
    <div class="sidebar-header">
        <h5 class="system-title">SGA</h5>
    </div>
    <nav class="navigation-menu">
        <a class="nav-link-item" href="#" onclick="navigate('home')">
            <i class="bi bi-house-door-fill"></i>
            <span>Início</span>
        </a>
        <a class="nav-link-item" href="#" onclick="navigate('operacao')">
            <i class="bi bi-arrow-left-right"></i>
            <span>Operação</span>
        </a>
        <a class="nav-link-item" href="#" onclick="navigate('alunos')">
            <i class="bi bi-people-fill"></i>
            <span>Alunos</span>
        </a>
        <a class="nav-link-item" href="#" onclick="navigate('computadores')">
            <i class="bi bi-pc-display-horizontal"></i>
            <span>Computadores</span>
        </a>
        <a class="nav-link-item" href="#" onclick="navigate('logs')">
            <i class="bi bi-clipboard-data-fill"></i>
            <span>Logs</span>
        </a>
    </nav>
    <div class="sidebar-footer">
        <small>SGA ESP32</small>
    </div>
</div>

<main class="main-content" id="mainContent">
    <!-- O conteúdo será injetado aqui pelo JavaScript -->
</main>

<!-- Offcanvas para menu mobile -->
<div class="offcanvas offcanvas-start offcanvas-glass d-lg-none" tabindex="-1" id="mobileMenu">
    <div class="offcanvas-header">
        <h5 class="system-title">SGA</h5>
        <button type="button" class="btn-close-glass" data-bs-dismiss="offcanvas"></button>
    </div>
    <div class="offcanvas-body">
        <nav class="navigation-menu-mobile">
            <a class="nav-link-item" href="#" onclick="navigate('home', true)"><i class="bi bi-house-door-fill me-2"></i>Início</a>
            <a class="nav-link-item" href="#" onclick="navigate('operacao', true)"><i class="bi bi-arrow-left-right me-2"></i>Operação</a>
            <a class="nav-link-item" href="#" onclick="navigate('alunos', true)"><i class="bi bi-people-fill me-2"></i>Alunos</a>
            <a class="nav-link-item" href="#" onclick="navigate('computadores', true)"><i class="bi bi-pc-display-horizontal me-2"></i>Computadores</a>
            <a class="nav-link-item" href="#" onclick="navigate('logs', true)"><i class="bi bi-clipboard-data-fill me-2"></i>Logs</a>
        </nav>
    </div>
</div>

<div id="toast-container"></div>
<div id="modals-placeholder"></div>

<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js"></script>
<script>
    const modalsHTML = `
        <div class="modal fade" id="readTagModal" tabindex="-1" data-bs-backdrop="static" data-bs-keyboard="false">
            <div class="modal-dialog modal-dialog-centered">
                <div class="modal-content modal-glass">
                    <div class="modal-header"><h5 class="modal-title">Aguardando Leitura da TAG</h5></div>
                    <div class="modal-body text-center">
                        <p>Aproxime a TAG/carteirinha do leitor...</p>
                        <div class="spinner-border text-primary my-3" role="status"><span class="visually-hidden">Aguardando...</span></div>
                    </div>
                    <div class="modal-footer"><button type="button" class="btn-glass-secondary" onclick="cancelarLeituraTag()">Cancelar</button></div>
                </div>
            </div>
        </div>
        <div class="modal fade" id="alunoModal" tabindex="-1">
            <div class="modal-dialog modal-dialog-centered modal-lg">
                <div class="modal-content modal-glass">
                    <div class="modal-header"><h5 class="modal-title" id="alunoModalLabel">Cadastrar Aluno</h5><button type="button" class="btn-close-glass" data-bs-dismiss="modal"></button></div>
                    <div class="modal-body">
                        <form id="form-aluno" onsubmit="event.preventDefault(); salvarAluno();">
                            <input type="hidden" id="aluno-uuid-original">
                            <div class="mb-3"><label class="form-label">Nome Completo</label><input type="text" class="form-control-glass" id="aluno-nome" placeholder="Nome do aluno" required></div>
                            <div class="mb-3"><label class="form-label">Matrícula</label><input type="text" class="form-control-glass" id="aluno-matricula" placeholder="Matrícula" required></div>
                            <div class="mb-3"><label class="form-label">UUID da Carteirinha</label><div class="input-group-glass"><input type="text" class="form-control-glass" id="aluno-uuid" placeholder="Aguardando leitura..." readonly required><button class="btn-glass-secondary" type="button" onclick="iniciarLeituraTag('alunoModal', 'aluno-uuid')">Ler TAG</button></div></div>
                        </form>
                    </div>
                    <div class="modal-footer"><button type="button" class="btn-glass-secondary" data-bs-dismiss="modal">Cancelar</button><button type="button" class="btn-glass-primary" onclick="salvarAluno()">Salvar</button></div>
                </div>
            </div>
        </div>
        <div class="modal fade" id="computadorModal" tabindex="-1">
            <div class="modal-dialog modal-dialog-centered modal-lg">
                <div class="modal-content modal-glass">
                    <div class="modal-header"><h5 class="modal-title" id="computadorModalLabel">Cadastrar Computador</h5><button type="button" class="btn-close-glass" data-bs-dismiss="modal"></button></div>
                    <div class="modal-body">
                        <form id="form-computador" onsubmit="event.preventDefault(); salvarComputador();">
                           <input type="hidden" id="comp-uuid-original">
                            <div class="mb-3"><label class="form-label">Nº Patrimônio</label><input type="text" class="form-control-glass" id="comp-patrimonio" placeholder="Ex: NTB001" required></div>
                            <div class="mb-3"><label class="form-label">Descrição</label><input type="text" class="form-control-glass" id="comp-descricao" placeholder="Ex: Dell Vostro 3400, i5 8GB" required></div>
                            <div class="mb-3"><label class="form-label">UUID da TAG</label><div class="input-group-glass"><input type="text" class="form-control-glass" id="comp-uuid" placeholder="Aguardando leitura..." readonly required><button class="btn-glass-secondary" type="button" onclick="iniciarLeituraTag('computadorModal', 'comp-uuid')">Ler TAG</button></div></div>
                        </form>
                    </div>
                    <div class="modal-footer"><button type="button" class="btn-glass-secondary" data-bs-dismiss="modal">Cancelar</button><button type="button" class="btn-glass-primary" onclick="salvarComputador()">Salvar</button></div>
                </div>
            </div>
        </div>
        <div class="modal fade" id="confirmDeleteModal" tabindex="-1">
            <div class="modal-dialog modal-dialog-centered">
                <div class="modal-content modal-glass">
                    <div class="modal-header"><h5 class="modal-title">Confirmar Exclusão</h5></div>
                    <div class="modal-body"><p>Tem certeza que deseja excluir <strong id="item-para-excluir">este item</strong>? Esta ação não pode ser desfeita.</p></div>
                    <div class="modal-footer"><button type="button" class="btn-glass-secondary" data-bs-dismiss="modal">Cancelar</button><button type="button" class="btn-glass-danger" id="btnConfirmarExclusao">Excluir</button></div>
                </div>
            </div>
        </div>
    `;
    document.getElementById('modals-placeholder').innerHTML = modalsHTML;

    let appState = {
        operacaoContinua: false,
        paginaAtual: 'home',
        operacaoStatusPoll: null,
        leituraTagPoll: null,
        modalOrigemId: null
    };

    const pages = {
        home: `
            <header class="main-header">
                <button class="mobile-menu-toggle d-lg-none" type="button" data-bs-toggle="offcanvas" data-bs-target="#mobileMenu"><span class="navbar-toggler-icon"></span></button>
                <h1 class="page-title">Sistema de Gestão de Ativos</h1>
            </header>
            <div class="container-fluid content-body">
                <div class="row g-4">
                    <div class="col-md-6"><div class="info-card card-glass h-100"><div class="info-card-header"><i class="bi bi-pc-display-horizontal info-card-icon"></i><h3 class="info-card-title">Computadores</h3></div><div class="info-card-body"><p class="info-card-text">Total de Ativos: <strong id="total-comps">0</strong></p><p class="info-card-text">Disponíveis: <strong class="text-success" id="comps-disponiveis">0</strong></p><p class="info-card-text">Emprestados: <strong class="text-danger" id="comps-emprestados">0</strong></p></div></div></div>
                    <div class="col-md-6"><div class="info-card card-glass h-100"><div class="info-card-header"><i class="bi bi-people-fill info-card-icon"></i><h3 class="info-card-title">Alunos</h3></div><div class="info-card-body"><p class="info-card-text">Total Cadastrados: <strong id="total-alunos">0</strong></p><p class="info-card-text">Com Empréstimo: <strong id="alunos-com-emprestimo">0</strong></p></div></div></div>
                </div>
            </div>`,
        operacao: `
            <header class="main-header"><h1 class="page-title">Empréstimo e Devolução</h1></header>
            <div class="container-fluid content-body"><div class="row justify-content-center"><div class="col-xl-8 col-lg-10"><div class="card-glass p-4">
                <div class="text-center mb-4"><i class="bi bi-arrow-left-right operation-icon"></i><h2 class="operation-title">Operação Contínua</h2><p class="operation-subtitle">Ative para processar empréstimos e devoluções em série.</p></div>
                <div id="operacao-log" class="mb-3 p-3" style="min-height: 100px; background: rgba(0,0,0,0.1); border-radius: 1rem; color: white;"></div>
                <div class="d-grid gap-3 mt-4">
                    <button class="btn-glass-primary" id="btnIniciarOperacaoContinua" onclick="toggleOperacaoContinua(true)"><i class="bi bi-play-circle-fill me-2"></i> Iniciar Operação</button>
                    <button class="btn-glass-danger" id="btnPararOperacaoContinua" onclick="toggleOperacaoContinua(false)" style="display:none;"><i class="bi bi-stop-circle-fill me-2"></i> Parar Operação</button>
                </div>
            </div></div></div></div>`,
        alunos: `
            <header class="main-header"><h1 class="page-title">Alunos Cadastrados</h1></header>
            <div class="container-fluid content-body">
                <div class="card-glass p-3 mb-4"><div class="d-flex justify-content-end align-items-center"><button class="btn-glass-primary" id="btn-novo-aluno" onclick="abrirModalAluno()"><i class="bi bi-plus-circle-fill me-2"></i>Novo Aluno</button></div></div>
                <div class="table-container-glass mt-3"><table class="table-glass"><thead><tr><th>Nome</th><th>Matrícula</th><th>UUID Carteirinha</th><th>Ações</th></tr></thead><tbody id="tabela-alunos"></tbody></table></div>
            </div>`,
        computadores: `
            <header class="main-header"><h1 class="page-title">Computadores Cadastrados</h1></header>
            <div class="container-fluid content-body">
                <div class="card-glass p-3 mb-4"><div class="d-flex justify-content-end align-items-center"><button class="btn-glass-primary" id="btn-novo-comp" onclick="abrirModalComputador()"><i class="bi bi-plus-circle-fill me-2"></i>Novo Computador</button></div></div>
                <div class="table-container-glass mt-3"><table class="table-glass"><thead><tr><th>Patrimônio</th><th>Descrição</th><th>Status</th><th>Ações</th></tr></thead><tbody id="tabela-computadores"></tbody></table></div>
            </div>`,
        logs: `
            <header class="main-header"><h1 class="page-title">Logs do Sistema</h1></header>
            <div class="container-fluid content-body">
                <div class="table-container-glass"><table class="table-glass"><thead><tr><th>Timestamp</th><th>Ação</th><th>Item</th><th>Responsável</th><th>Detalhes</th></tr></thead><tbody id="tabela-logs"></tbody></table></div>
            </div>`
    };

    function navigate(page, fromMobile = false) {
        if(fromMobile) {
            const mobileMenuEl = document.getElementById('mobileMenu');
            if(mobileMenuEl) {
                const bsOffcanvas = bootstrap.Offcanvas.getInstance(mobileMenuEl);
                if(bsOffcanvas) bsOffcanvas.hide();
            }
        }
        appState.paginaAtual = page;
        document.getElementById('mainContent').innerHTML = pages[page];
        updateActiveLink(page);
        atualizarUI(); 
    }
    
    function atualizarUI() {
        const page = appState.paginaAtual;
        if (page === 'home') carregarStatsHome();
        else if (page === 'alunos') carregarAlunos();
        else if (page === 'computadores') carregarComputadores();
        else if (page === 'logs') carregarLogs();
        else if (page === 'operacao') updateOperacaoUI();

        document.querySelectorAll('#btn-novo-aluno, #btn-novo-comp').forEach(btn => {
            if(btn) btn.disabled = appState.operacaoContinua;
        });
    }

    function updateActiveLink(page) {
        document.querySelectorAll('.navigation-menu .nav-link-item, .navigation-menu-mobile .nav-link-item').forEach(link => {
            link.classList.remove('active');
            const pageName = link.getAttribute('onclick').match(/'(.*?)'/)[1];
            if (pageName === page) link.classList.add('active');
        });
    }

    async function carregarAlunos() {
        try {
            const response = await fetch('/api/alunos?_t=' + new Date().getTime());
            const alunos = await response.json();
            const tabela = document.getElementById('tabela-alunos');
            if (!tabela) return;
            tabela.innerHTML = '';
            alunos.forEach(aluno => {
                const row = tabela.insertRow();
                row.innerHTML = `
                    <td>${aluno.nome}</td>
                    <td>${aluno.matricula}</td>
                    <td>${aluno.uuid_carteirinha}</td>
                    <td>
                        <button class="btn-icon-glass" onclick="abrirModalAluno('${aluno.uuid_carteirinha}')"><i class="bi bi-pencil-fill"></i></button>
                        <button class="btn-icon-glass-danger" onclick="confirmarExclusao('aluno', '${aluno.uuid_carteirinha}', '${aluno.nome}')"><i class="bi bi-trash-fill"></i></button>
                    </td>`;
            });
        } catch (e) { console.error("Falha ao carregar alunos", e); }
    }

    async function carregarComputadores() {
        try {
            const ts = new Date().getTime();
            const [compsRes, emprestimosRes] = await Promise.all([ 
                fetch('/api/computadores?_t=' + ts), 
                fetch('/api/emprestimos_ativos?_t=' + ts) 
            ]);
            const computadores = await compsRes.json();
            const emprestimos = await emprestimosRes.json();
            const emprestadosUUIDs = emprestimos.map(e => e.uuid_computador);

            const tabela = document.getElementById('tabela-computadores');
            if(!tabela) return;
            tabela.innerHTML = '';
            computadores.forEach(comp => {
                const isEmprestado = emprestadosUUIDs.includes(comp.uuid_tag);
                const statusBadge = isEmprestado ? '<span class="badge-glass-danger">Emprestado</span>' : '<span class="badge-glass-success">Disponível</span>';
                const row = tabela.insertRow();
                row.innerHTML = `
                    <td>${comp.patrimonio}</td>
                    <td>${comp.descricao}</td>
                    <td>${statusBadge}</td>
                    <td>
                        <button class="btn-icon-glass" onclick="abrirModalComputador('${comp.uuid_tag}')"><i class="bi bi-pencil-fill"></i></button>
                        <button class="btn-icon-glass-danger" onclick="confirmarExclusao('computador', '${comp.uuid_tag}', '${comp.patrimonio}')"><i class="bi bi-trash-fill"></i></button>
                    </td>`;
            });
        } catch (e) { console.error("Falha ao carregar computadores", e); }
    }
    
    async function carregarLogs() {
        try {
            const response = await fetch('/api/logs?_t=' + new Date().getTime());
            const logs = await response.json();
            const tabela = document.getElementById('tabela-logs');
            if(!tabela) return;
            tabela.innerHTML = '';
            logs.reverse().forEach(log => {
                const row = tabela.insertRow();
                let acaoBadge;
                if (log.acao === 'emprestimo') acaoBadge = '<span class="badge-glass-info">Empréstimo</span>';
                else if (log.acao === 'devolucao') acaoBadge = '<span class="badge-glass-success">Devolução</span>';
                else acaoBadge = `<span class="badge-glass-warning">${log.acao || 'Sistema'}</span>`;
                
                row.innerHTML = `
                    <td>${new Date(log.timestamp).toLocaleString('pt-BR')}</td>
                    <td>${acaoBadge}</td>
                    <td>${log.item_patrimonio || '-'}</td>
                    <td>${log.responsavel_nome || 'Sistema'}</td>
                    <td>${log.item_descricao || log.responsavel_matricula || ''}</td>`;
            });
        } catch (e) { console.error("Falha ao carregar logs", e); }
    }

    async function carregarStatsHome() {
        try {
            const ts = new Date().getTime();
            const [alunosRes, compsRes, emprestimosRes] = await Promise.all([ 
                fetch('/api/alunos?_t=' + ts), 
                fetch('/api/computadores?_t=' + ts), 
                fetch('/api/emprestimos_ativos?_t=' + ts) 
            ]);
            const alunos = await alunosRes.json();
            const computadores = await compsRes.json();
            const emprestimos = await emprestimosRes.json();
            
            document.getElementById('total-alunos').textContent = alunos.length;
            document.getElementById('total-comps').textContent = computadores.length;
            document.getElementById('comps-emprestados').textContent = emprestimos.length;
            document.getElementById('alunos-com-emprestimo').textContent = emprestimos.length;
            document.getElementById('comps-disponiveis').textContent = computadores.length - emprestimos.length;
        } catch (e) { console.error("Falha ao carregar estatísticas", e); }
    }
    
    let campoAlvoTag = null;
    let readTagModalInstance = null;
    
    async function iniciarLeituraTag(modalOrigemId, campoId) {
        if(appState.operacaoContinua) { showToast("Cadastro bloqueado durante a operação contínua.", true); return; }
        
        campoAlvoTag = campoId;
        appState.modalOrigemId = modalOrigemId;
        const modalOrigemEl = document.getElementById(appState.modalOrigemId);
        if(modalOrigemEl) bootstrap.Modal.getInstance(modalOrigemEl)?.hide();

        const readTagModalEl = document.getElementById('readTagModal');
        readTagModalInstance = new bootstrap.Modal(readTagModalEl);
        
        readTagModalEl.addEventListener('shown.bs.modal', async () => {
             try {
                await fetch('/api/iniciar_sessao_nfc');
                appState.leituraTagPoll = setInterval(verificarTagLidaParaCadastro, 1000);
            } catch (e) {
                showToast('Erro: ' + e.message, true); 
                cancelarLeituraTag();
            }
        }, { once: true });
        
        readTagModalInstance.show();
    }

    async function verificarTagLidaParaCadastro() {
        if(!appState.leituraTagPoll) return;
        try {
            const response = await fetch('/api/get_nfc_tag');
            
            if (!response.ok) {
                clearInterval(appState.leituraTagPoll);
                appState.leituraTagPoll = null;
                const errorData = await response.json().catch(() => ({ error: 'Falha de comunicação com o dispositivo.' }));
                showToast(errorData.error || 'Erro desconhecido do servidor.', true);
                cancelarLeituraTag();
                return;
            }

            const data = await response.json();
            if (data && data.uuid_lida) {
                clearInterval(appState.leituraTagPoll);
                appState.leituraTagPoll = null;
                if(campoAlvoTag) document.getElementById(campoAlvoTag).value = data.uuid_lida;
                
                const modalOrigemEl = document.getElementById(appState.modalOrigemId);
                readTagModalInstance.hide();
                const readTagModalEl = document.getElementById('readTagModal');
                readTagModalEl.addEventListener('hidden.bs.modal', () => {
                    if(modalOrigemEl) new bootstrap.Modal(modalOrigemEl).show();
                }, { once: true });

                await cancelarSessaoNFC();
            }
        } catch(e) { 
            console.error("Erro fatal no polling de tag", e);
            showToast("Erro de rede ao buscar TAG.", true);
            cancelarLeituraTag();
        }
    }

    function cancelarLeituraTag() {
        if(appState.leituraTagPoll) clearInterval(appState.leituraTagPoll);
        appState.leituraTagPoll = null;
        if(readTagModalInstance) readTagModalInstance.hide();
        
        const readTagModalEl = document.getElementById('readTagModal');
        readTagModalEl.addEventListener('hidden.bs.modal', () => {
             const modalOrigemEl = document.getElementById(appState.modalOrigemId);
             if(modalOrigemEl && bootstrap.Modal.getInstance(modalOrigemEl) === null) {
                new bootstrap.Modal(modalOrigemEl).show();
             }
        }, { once: true });

        cancelarSessaoNFC();
    }

    async function cancelarSessaoNFC() { await fetch('/api/cancelar_sessao_nfc'); }
    
    async function abrirModalAluno(uuid = null) {
        if(appState.operacaoContinua) { showToast("Cadastro bloqueado durante a operação contínua.", true); return; }
        const modal = new bootstrap.Modal(document.getElementById('alunoModal'));
        document.getElementById('form-aluno').reset();
        document.getElementById('alunoModalLabel').textContent = 'Novo Aluno';
        document.getElementById('aluno-uuid-original').value = '';
        document.getElementById('aluno-uuid').value = '';

        if (uuid) {
            const response = await fetch(`/api/aluno?uuid=${uuid}&_t=${new Date().getTime()}`);
            const aluno = await response.json();
            document.getElementById('alunoModalLabel').textContent = 'Editar Aluno';
            document.getElementById('aluno-nome').value = aluno.nome;
            document.getElementById('aluno-matricula').value = aluno.matricula;
            document.getElementById('aluno-uuid').value = aluno.uuid_carteirinha;
            document.getElementById('aluno-uuid-original').value = aluno.uuid_carteirinha;
        }
        modal.show();
    }

    async function salvarAluno() {
        const uuid = document.getElementById('aluno-uuid').value;
        if (!uuid) { showToast("É necessário ler a TAG da carteirinha.", true); return; }
        const uuidOriginal = document.getElementById('aluno-uuid-original').value;
        const data = {
            nome: document.getElementById('aluno-nome').value,
            matricula: document.getElementById('aluno-matricula').value,
            uuid_carteirinha: uuid,
            uuid_original: uuidOriginal
        };

        const response = await fetch(uuidOriginal ? '/api/update_aluno' : '/api/add_aluno', {
            method: uuidOriginal ? 'PUT' : 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        });
        const result = await response.json();
        showToast(result.message || result.error, !response.ok);
        if (response.ok) {
            bootstrap.Modal.getInstance(document.getElementById('alunoModal')).hide();
            atualizarUI();
        }
    }

    async function abrirModalComputador(uuid = null) {
        if(appState.operacaoContinua) { showToast("Cadastro bloqueado durante a operação contínua.", true); return; }
        const modal = new bootstrap.Modal(document.getElementById('computadorModal'));
        document.getElementById('form-computador').reset();
        document.getElementById('computadorModalLabel').textContent = 'Novo Computador';
        document.getElementById('comp-uuid-original').value = '';
        document.getElementById('comp-uuid').value = '';
        
        if (uuid) {
            const response = await fetch(`/api/computador?uuid=${uuid}&_t=${new Date().getTime()}`);
            const comp = await response.json();
            document.getElementById('computadorModalLabel').textContent = 'Editar Computador';
            document.getElementById('comp-patrimonio').value = comp.patrimonio;
            document.getElementById('comp-descricao').value = comp.descricao;
            document.getElementById('comp-uuid').value = comp.uuid_tag;
            document.getElementById('comp-uuid-original').value = comp.uuid_tag;
        }
        modal.show();
    }

    async function salvarComputador() {
        const uuid = document.getElementById('comp-uuid').value;
        if (!uuid) { showToast("É necessário ler a TAG do computador.", true); return; }
        const uuidOriginal = document.getElementById('comp-uuid-original').value;
        const data = {
            patrimonio: document.getElementById('comp-patrimonio').value,
            descricao: document.getElementById('comp-descricao').value,
            uuid_tag: uuid,
            uuid_original: uuidOriginal
        };
        
        const response = await fetch(uuidOriginal ? '/api/update_computador' : '/api/add_computador', {
            method: uuidOriginal ? 'PUT' : 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        });
        const result = await response.json();
        showToast(result.message || result.error, !response.ok);
        if (response.ok) {
            bootstrap.Modal.getInstance(document.getElementById('computadorModal')).hide();
            atualizarUI();
        }
    }
    
    function confirmarExclusao(tipo, uuid, nome) {
        if(appState.operacaoContinua) { showToast("Ações bloqueadas durante a operação contínua.", true); return; }
        const modal = new bootstrap.Modal(document.getElementById('confirmDeleteModal'));
        document.getElementById('item-para-excluir').textContent = nome;
        document.getElementById('btnConfirmarExclusao').onclick = () => deletarItem(tipo, uuid);
        modal.show();
    }
    
    async function deletarItem(tipo, uuid) {
        const url = `/api/delete_${tipo}?uuid=${uuid}`;
        const response = await fetch(url, { method: 'DELETE' });
        const result = await response.json();
        
        showToast(result.message || result.error, !response.ok);
        if (response.ok) {
            bootstrap.Modal.getInstance(document.getElementById('confirmDeleteModal')).hide();
            atualizarUI();
        }
    }
    
    function updateOperacaoUI() {
        const btnIniciar = document.getElementById('btnIniciarOperacaoContinua');
        const btnParar = document.getElementById('btnPararOperacaoContinua');
        if (appState.operacaoContinua) {
            if (btnIniciar) btnIniciar.style.display = 'none';
            if (btnParar) btnParar.style.display = 'block';
        } else {
            if (btnIniciar) btnIniciar.style.display = 'block';
            if (btnParar) btnParar.style.display = 'none';
        }
    }

    async function toggleOperacaoContinua(iniciar) {
        try {
            await fetch('/api/toggle_operacao', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({ ativo: iniciar })
            });
            appState.operacaoContinua = iniciar;
            updateOperacaoUI();
            atualizarUI();

            if (iniciar) {
                appState.operacaoStatusPoll = setInterval(pollStatusOperacao, 1500);
            } else {
                clearInterval(appState.operacaoStatusPoll);
                appState.operacaoStatusPoll = null;
                const operacaoLog = document.getElementById('operacao-log');
                if(operacaoLog) operacaoLog.innerHTML = '';
            }
        } catch (e) {
            showToast("Falha ao alterar modo de operação", true);
        }
    }
    
    async function pollStatusOperacao() {
        if(!appState.operacaoContinua) {
            clearInterval(appState.operacaoStatusPoll);
            appState.operacaoStatusPoll = null;
            return;
        }
        try {
            const response = await fetch('/api/status_operacao?_t=' + new Date().getTime());
            const data = await response.json();
            if(appState.paginaAtual === 'operacao'){
                const operacaoLog = document.getElementById('operacao-log');
                if (operacaoLog) {
                    let statusHtml = '';
                    if(data.ultimoResultado) {
                        const statusClass = data.ultimoResultado.startsWith("ERRO:") ? "text-danger" : "text-success";
                        statusHtml += `<p class="${statusClass}">${data.ultimoResultado}</p>`;
                    }
                    statusHtml += `<p>Aguardando: <strong>${data.estadoAtual}</strong></p>`;
                    if(data.alunoSessao) statusHtml += `<p>Aluno na sessão: ${data.alunoSessao}</p>`;
                    operacaoLog.innerHTML = statusHtml;
                }
            }
            if(data.toastMessage) {
                showToast(data.toastMessage, data.toastMessage.startsWith("ERRO:"));
            }
        } catch(e) { /* Silencioso */ }
    }
    
    function showToast(message, isError = false) {
        const container = document.getElementById('toast-container');
        const toast = document.createElement('div');
        toast.className = `toast-item ${isError ? 'error' : 'success'}`;
        toast.textContent = message;
        container.appendChild(toast);
        setTimeout(() => toast.classList.add('show'), 10);
        setTimeout(() => {
            toast.classList.remove('show');
            setTimeout(() => container.removeChild(toast), 500);
        }, 4000);
    }

    document.addEventListener('DOMContentLoaded', async () => {
        try {
            const response = await fetch('/api/status_operacao?_t=' + new Date().getTime());
            const data = await response.json();
            appState.operacaoContinua = data.operacaoAtiva;
            if(appState.operacaoContinua) {
                 appState.operacaoStatusPoll = setInterval(pollStatusOperacao, 1500);
            }
        } catch(e) { console.error("Falha ao buscar estado inicial da operação", e); }
        navigate('home');
    });
</script>

</body>
</html>
)rawliteral";


// =================================================================
// MARK: - CONFIGURAÇÕES E CONSTANTES GLOBAIS
// =================================================================

// --- Credenciais de Wi-Fi ---
// O sistema tentará conectar às redes nesta ordem.
struct WiFiCredential {
  const char* ssid;
  const char* password;
};

WiFiCredential wifiNetworks[] = {
  {"roteador", "12345678"},
};

const int numWifiNetworks = sizeof(wifiNetworks) / sizeof(wifiNetworks[0]);

// --- Pinos do Hardware ---
const int PN532_SDA_PIN = 22;
const int PN532_SCL_PIN = 23;
const int PN532_IRQ_PIN = -1; // Não utilizado
const int PN532_RST_PIN = -1; // Não utilizado

const int LED_VERDE_PIN = 6;
const int LED_AMARELO_PIN = 5;
const int LED_VERMELHO_PIN = 4;
const int BUZZER_PIN = 20;
bool primeiraVezligado = true;

// --- Definições das Notas (Frequências em Hz) ---
#define REST      0
#define NOTE_C4   262
#define NOTE_D4   294
#define NOTE_E4   330
#define NOTE_F4   349
#define NOTE_G4   392
#define NOTE_A4   440
#define NOTE_B4   494
#define NOTE_C5   523
#define NOTE_E5   659
#define NOTE_G5   784
#define NOTE_DS3  156
#define NOTE_C6   1047
#define NOTE_D3   147

// --- Nomes dos Arquivos no Sistema LittleFS ---
const char* ALUNOS_FILE = "/alunos.json";
const char* COMPUTADORES_FILE = "/computadores.json";
const char* LOGS_FILE = "/logs.json";
const char* EMPRESTIMOS_ATIVOS_FILE = "/emprestimos_ativos.json";

// --- Configurações de Tempo ---
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -10800; // Fuso de Brasília (GMT-3)
const int daylightOffset_sec = 0;   // Sem horário de verão


// =================================================================
// MARK: - VARIÁVEIS GLOBAIS DE ESTADO
// =================================================================

// --- Objetos de Hardware e Servidor ---
WebServer httpServer(80);
Adafruit_PN532 nfc(PN532_SDA_PIN, PN532_SCL_PIN);

// Configurações do PWM
const int PWM_FREQ = 5000;
const int PWM_RESOLUTION = 8;

// --- Máquina de Estados para Operação Contínua ---
enum class EstadoOperacao { OCIOSO, AGUARDANDO_ALUNO, AGUARDANDO_COMPUTADOR };
EstadoOperacao estadoOperacao = EstadoOperacao::OCIOSO;

bool modoOperacaoContinua = false;     // Flag que controla o modo de operação principal
String alunoUUID_sessao = "";          // Armazena o UUID do aluno durante uma transação
String alunoNome_sessao = "";          // Armazena o nome do aluno durante uma transação
String ultimoResultado_sessao = "";    // Guarda a última mensagem de status para a UI

// --- Variáveis para Leitura de TAG para Cadastro (Polling) ---
String nfcUidParaPolling = "";       // Guarda o UID lido que aguarda ser pego pela UI
String ultimoUidNFC = "";       // Guarda o ultimo UID lido que aguarda ser pego pela UI
bool aguardandoLeituraPolling = false; // Flag que ativa a leitura de TAG para formulários
bool pn532_initialized = false;      // Indica se o leitor NFC foi inicializado com sucesso
unsigned long lastNFCReadAttempt = 0;  // Controle de tempo para não sobrecarregar o leitor
const unsigned long nfcReadInterval = 300; // Intervalo em ms entre as tentativas de leitura


// =================================================================
// MARK: - PROTÓTIPOS DE FUNÇÕES
// =================================================================

// --- Inicialização ---
void initLittleFS();
void initWiFi();
void initPN532();
void initFeedbackPins();
void configureServerRoutes();

// --- Feedback Visual e Sonoro ---
void sinalizarSucesso();
void sinalizarErro();
void sinalizarStandby();
void tocarMelodiaSucesso();
void tocarMelodiaErro();
void tocarMelodiaInicializacao();
void tocarMelodiaLeitura();


// --- Leitor NFC ---
String readNFCUID();
void processarLeituraContinua();

// --- Manipulação de Arquivos JSON (CRUD no LittleFS) ---
void serveFile(const char* filename, const char* contentType);
bool readJsonFile(const char* filename, DynamicJsonDocument& doc);
bool writeJsonFile(const char* filename, const DynamicJsonDocument& doc);
bool appendToJsonArray(const char* filename, const JsonObject& newData);
bool updateRecordInJsonArray(const char* filename, const char* keyToMatch, const String& valueToMatch, const JsonObject& newData);
bool deleteRecordFromJsonArray(const char* filename, const char* keyToMatch, const String& valueToMatch);
bool getRecordFromJsonArray(const char* filename, const char* keyToMatch, const String& valueToMatch, JsonObject& recordFound);

// --- Lógica de Negócio (Regras de Empréstimo) ---
String executarTransacao(String alunoUUID, String compUUID);
bool addEmprestimoAtivo(const JsonObject& emprestimoData);
bool removeEmprestimoAtivo(const String& uuid_computador);
bool getAlunoInfoByUUID(const String& uuid_carteirinha, String& nome, String& matricula);
bool getComputadorInfoByUUID(const String& uuid_tag, String& patrimonio, String& descricao);
bool isAlunoComEmprestimo(const String& uuid_aluno, String& emprestado_uuid_computador, String& timestamp_emprestimo);
bool isComputadorEmprestado(const String& uuid_computador, String& emprestado_para_uuid_aluno);

// --- Log e Timestamp ---
String getReliableTimestamp(time_t &unix_time);
void registrarLogTransacao(const String& acao, const String& alunoUUID, const String& alunoNome, const String& alunoMatricula, const String& compUUID, const String& compPatrimonio, const String& compDescricao);

// --- Handlers da API (Endpoints Web) ---
void handleApiGetAluno();
void handleApiAddAluno();
void handleApiUpdateAluno();
void handleApiDeleteAluno();
void handleApiGetComputador();
void handleApiAddComputador();
void handleApiUpdateComputador();
void handleApiDeleteComputador();
void handleApiIniciarSessaoNFC();
void handleApiGetNFCTag();
void handleApiCancelarSessaoNFC();
void handleToggleOperacaoContinua();
void handleApiGetStatusOperacao();
void handleNotFound();


// =================================================================
// MARK: - FUNÇÕES DE FEEDBACK VISUAL E SONORO
// =================================================================


/**
 * @brief Toca uma melodia para inicialização.
 */
void tocarMelodiaInicializacao() {
  int melody[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, REST, NOTE_G4, NOTE_C5};
  int durations[] = {100, 100, 100, 150, 50, 100, 250};

  for (int i = 0; i < 7; i++) {
    ledcWriteTone(BUZZER_PIN, melody[i]);
    delay(durations[i]);
  }
  // Desliga o buzzer
  ledcWrite(BUZZER_PIN, 0);
}


/**
 * @brief Toca uma melodia de sucesso.
 */
void tocarMelodiaSucesso() {
  int melody[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5};
  int durations[] = {100, 100, 100, 250};

  for (int i = 0; i < 4; i++) {
    ledcWriteTone(BUZZER_PIN, melody[i]);
    delay(durations[i]);
  }
  // Desliga o buzzer
  ledcWrite(BUZZER_PIN, 0);
}

/**
 * @brief Toca uma melodia para indicar um erro.
 */
void tocarMelodiaErro() {
  int melody[] = {NOTE_D3, NOTE_DS3, NOTE_D3};
  int durations[] = {150, 150, 150};

  for (int i = 0; i < 3; i++) {
    ledcWriteTone(BUZZER_PIN, melody[i]);
    delay(durations[i]);
  }
  // Desliga o buzzer
  ledcWrite(BUZZER_PIN, 0);
}

void tocarMelodiaLeitura() {
  ledcWriteTone(BUZZER_PIN, NOTE_C6);
  delay(100);
  ledcWrite(BUZZER_PIN, 0);
}


void sinalizarSucesso() {
	if (LED_VERDE_PIN >= 0) digitalWrite(LED_VERDE_PIN, HIGH);
	if (LED_VERMELHO_PIN >= 0) digitalWrite(LED_VERMELHO_PIN, LOW);
	tocarMelodiaSucesso();
}

void sinalizarErro() {
	if (LED_VERMELHO_PIN >= 0) digitalWrite(LED_VERMELHO_PIN, HIGH);
	if (LED_VERDE_PIN >= 0) digitalWrite(LED_VERDE_PIN, LOW);
    tocarMelodiaErro();
}

void sinalizarStandby() {
	if (LED_AMARELO_PIN >= 0) digitalWrite(LED_AMARELO_PIN, HIGH);
	if (LED_VERDE_PIN >= 0) digitalWrite(LED_VERDE_PIN, LOW);
	if (LED_VERMELHO_PIN >= 0) digitalWrite(LED_VERMELHO_PIN, LOW);
}

// =================================================================
// MARK: - FUNÇÕES DE MANIPULAÇÃO DE ARQUIVOS JSON
// =================================================================

/**
 * @brief Lê um arquivo JSON do sistema de arquivos e o carrega em um DynamicJsonDocument.
 */
bool readJsonFile(const char* filename, DynamicJsonDocument& doc) {
	File file = LittleFS.open(filename, "r");
	if (!file) { return false; }
	DeserializationError error = deserializeJson(doc, file);
	file.close();
	return !error;
}

/**
 * @brief Escreve o conteúdo de um DynamicJsonDocument em um arquivo JSON.
 */
bool writeJsonFile(const char* filename, const DynamicJsonDocument& doc) {
	File file = LittleFS.open(filename, "w");
	if (!file) { return false; }
	bool success = serializeJson(doc, file) > 0;
	file.close();
	return success;
}

/**
 * @brief Adiciona um novo objeto a um array JSON existente em um arquivo.
 */
bool appendToJsonArray(const char* filename, const JsonObject& newData) {
	DynamicJsonDocument doc(4096);
	JsonArray array;
	if (LittleFS.exists(filename) && readJsonFile(filename, doc) && doc.is<JsonArray>()) {
		array = doc.as<JsonArray>();
	} else {
		array = doc.to<JsonArray>();
	}
	array.add(newData);
	return writeJsonFile(filename, doc);
}

/**
 * @brief Atualiza um registro dentro de um array JSON com base em uma chave e valor.
 */
bool updateRecordInJsonArray(const char* filename, const char* keyToMatch, const String& valueToMatch, const JsonObject& newData) {
	DynamicJsonDocument doc(4096);
	if (!readJsonFile(filename, doc) || !doc.is<JsonArray>()) return false;
	
	JsonArray array = doc.as<JsonArray>();
	bool updated = false;
	for (JsonObject obj : array) {
		if (obj[keyToMatch] == valueToMatch) {
			for(JsonPair kv : newData) { obj[kv.key()] = kv.value(); }
			updated = true;
			break;
		}
	}
	return updated ? writeJsonFile(filename, doc) : false;
}

/**
 * @brief Deleta um registro de um array JSON com base em uma chave e valor.
 */
bool deleteRecordFromJsonArray(const char* filename, const char* keyToMatch, const String& valueToMatch) {
	DynamicJsonDocument doc(4096);
	if (!readJsonFile(filename, doc) || !doc.is<JsonArray>()) return false;
	
	JsonArray array = doc.as<JsonArray>();
	bool removed = false;
	for (int i = array.size() - 1; i >= 0; i--) {
		if (array[i][keyToMatch] == valueToMatch) {
			array.remove(i);
			removed = true;
		}
	}
	return removed ? writeJsonFile(filename, doc) : false;
}

/**
 * @brief Obtém um registro específico de um array JSON com base em uma chave e valor.
 */
bool getRecordFromJsonArray(const char* filename, const char* keyToMatch, const String& valueToMatch, JsonObject& recordFound) {
	DynamicJsonDocument doc(2048);
	if (!readJsonFile(filename, doc) || !doc.is<JsonArray>()) return false;

	for (JsonObject obj : doc.as<JsonArray>()) {
		if (obj[keyToMatch] == valueToMatch) {
			recordFound.clear();
			for(JsonPair kv : obj) { recordFound[kv.key()] = kv.value(); }
			return true;
		}
	}
	return false;
}


// =================================================================
// MARK: - LÓGICA DE NEGÓCIO (REGRAS DE EMPRÉSTIMO)
// =================================================================

/**
 * @brief Adiciona um novo registro de empréstimo ativo.
 */
bool addEmprestimoAtivo(const JsonObject& emprestimoData) {
	return appendToJsonArray(EMPRESTIMOS_ATIVOS_FILE, emprestimoData);
}

/**
 * @brief Remove um registro de empréstimo ativo.
 */
bool removeEmprestimoAtivo(const String& uuid_computador) {
	return deleteRecordFromJsonArray(EMPRESTIMOS_ATIVOS_FILE, "uuid_computador", uuid_computador);
}

/**
 * @brief Busca informações de um aluno pelo UUID da carteirinha.
 */
bool getAlunoInfoByUUID(const String& uuid_carteirinha, String& nome, String& matricula) {
	DynamicJsonDocument doc(512);
	JsonObject record = doc.to<JsonObject>();
	if (getRecordFromJsonArray(ALUNOS_FILE, "uuid_carteirinha", uuid_carteirinha, record)) {
		nome = record["nome"].as<String>();
		matricula = record["matricula"].as<String>();
		return true;
	}
	return false;
}

/**
 * @brief Busca informações de um computador pelo UUID da TAG.
 */
bool getComputadorInfoByUUID(const String& uuid_tag, String& patrimonio, String& descricao) {
	DynamicJsonDocument doc(512);
	JsonObject record = doc.to<JsonObject>();
	if (getRecordFromJsonArray(COMPUTADORES_FILE, "uuid_tag", uuid_tag, record)) {
		patrimonio = record["patrimonio"].as<String>();
		descricao = record["descricao"].as<String>();
		return true;
	}
	return false;
}

/**
 * @brief Verifica se um aluno já possui um empréstimo ativo.
 */
bool isAlunoComEmprestimo(const String& uuid_aluno, String& emprestado_uuid_computador, String& timestamp_emprestimo) {
	DynamicJsonDocument doc(4096);
	if (!readJsonFile(EMPRESTIMOS_ATIVOS_FILE, doc) || !doc.is<JsonArray>()) return false;
	for(JsonObject emprestimo : doc.as<JsonArray>()) {
		if (emprestimo["uuid_aluno_carteirinha"] == uuid_aluno) {
			emprestado_uuid_computador = emprestimo["uuid_computador"].as<String>();
			timestamp_emprestimo = emprestimo["timestamp_emprestimo"].as<String>();
			return true;
		}
	}
	return false;
}

/**
 * @brief Verifica se um computador já está emprestado.
 */
bool isComputadorEmprestado(const String& uuid_computador, String& emprestado_para_uuid_aluno) {
	DynamicJsonDocument doc(4096);
	if (!readJsonFile(EMPRESTIMOS_ATIVOS_FILE, doc) || !doc.is<JsonArray>()) return false;
	for(JsonObject emprestimo : doc.as<JsonArray>()) {
		if (emprestimo["uuid_computador"] == uuid_computador) {
			emprestado_para_uuid_aluno = emprestimo["uuid_aluno_carteirinha"].as<String>();
			return true;
		}
	}
	return false;
}

/**
 * @brief Processa uma transação de empréstimo ou devolução.
 * @return Uma string com o resultado da operação ("OK" ou "ERRO").
 */
String executarTransacao(String alunoUUID, String compUUID) {
    String alunoNome, alunoMatricula, compPatrimonio, compDescricao;
    bool alunoValido = getAlunoInfoByUUID(alunoUUID, alunoNome, alunoMatricula);
    bool compValido = getComputadorInfoByUUID(compUUID, compPatrimonio, compDescricao);

    if (!alunoValido) return "ERRO: Aluno com TAG nao cadastrado.";
    if (!compValido) return "ERRO: Computador com TAG nao cadastrado.";
    
    String compEmprestado, tsEmprestimo, alunoComComp;
    bool alunoTemItem = isAlunoComEmprestimo(alunoUUID, compEmprestado, tsEmprestimo);
    bool compEstaComAlguem = isComputadorEmprestado(compUUID, alunoComComp);

    // Cenário: DEVOLUÇÃO
    if (alunoTemItem && compEmprestado == compUUID) {
        if (removeEmprestimoAtivo(compUUID)) {
            registrarLogTransacao("devolucao", alunoUUID, alunoNome, alunoMatricula, compUUID, compPatrimonio, compDescricao);
            sinalizarSucesso();
            return "Devolucao OK: " + compDescricao;
        } else {
            sinalizarErro();
            return "ERRO: Falha de sistema ao processar devolucao.";
        }
    } 
    // Cenário: EMPRÉSTIMO
    else if (!alunoTemItem && !compEstaComAlguem) {
        DynamicJsonDocument emprestimoDoc(512); 
        JsonObject data = emprestimoDoc.to<JsonObject>();
        time_t now_ts = 0; 
        String timestamp = getReliableTimestamp(now_ts);
        if(timestamp == "") timestamp = "NO_NTP_TS";
        
        data["uuid_computador"] = compUUID; 
        data["patrimonio_computador"] = compPatrimonio;
        data["uuid_aluno_carteirinha"] = alunoUUID; 
        data["nome_aluno"] = alunoNome; 
        data["timestamp_emprestimo"] = timestamp;
        
        if (addEmprestimoAtivo(data)) {
            registrarLogTransacao("emprestimo", alunoUUID, alunoNome, alunoMatricula, compUUID, compPatrimonio, compDescricao);
            sinalizarSucesso();
            return "Emprestimo OK: " + compDescricao;
        } else {
            sinalizarErro();
            return "ERRO: Falha de sistema ao registrar emprestimo.";
        }
    } 
    // Cenários de ERRO DE LÓGICA
    else {
        sinalizarErro();
        if (alunoTemItem) {
            String patrimonioItemAtual, descItemAtual;
            if (getComputadorInfoByUUID(compEmprestado, patrimonioItemAtual, descItemAtual)) {
                return "ERRO: Aluno " + alunoNome + " ja possui o item " + descItemAtual + ".";
            } else {
                return "ERRO: Aluno " + alunoNome + " ja possui outro item " + descItemAtual + ".";
            }
        }
        if (compEstaComAlguem) {
             String nomeAlunoComItem, matriculaAlunoComItem;
             if(getAlunoInfoByUUID(alunoComComp, nomeAlunoComItem, matriculaAlunoComItem)){
                return "ERRO: " + compDescricao + " ja esta com o aluno " + nomeAlunoComItem + ".";
             } else {
                return "ERRO: " + compDescricao + " ja esta com outro aluno .";
             }
        }
        return "ERRO: Condicao de transacao desconhecida."; // Fallback
    }
}


// =================================================================
// MARK: - FUNÇÕES DE LOG E TIMESTAMP
// =================================================================

/**
 * @brief Obtém a data e hora atual do servidor NTP e formata em ISO 8601.
 */
String getReliableTimestamp(time_t &unix_time) {
	struct tm timeinfo;
	if (getLocalTime(&timeinfo, 5000)) {
		unix_time = mktime(&timeinfo);
		char buffer[25];
		strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &timeinfo);
		
		char offset_buffer[7];
		long offset_hours = gmtOffset_sec / 3600;
		long offset_mins = (abs(gmtOffset_sec) % 3600) / 60;
		snprintf(offset_buffer, sizeof(offset_buffer), "%+03ld:%02ld", offset_hours, offset_mins);

		return String(buffer) + String(offset_buffer);
	}
	return "";
}

/**
 * @brief Registra um evento (empréstimo/devolução) no arquivo de logs.
 */
void registrarLogTransacao(const String& acao, const String& alunoUUID, const String& alunoNome, const String& alunoMatricula, const String& compUUID, const String& compPatrimonio, const String& compDescricao) {
	DynamicJsonDocument logEntryDoc(1024);
	JsonObject logEntry = logEntryDoc.to<JsonObject>();
	time_t now_unix = 0;
	String timestamp_iso = getReliableTimestamp(now_unix);
	if (timestamp_iso.isEmpty()) timestamp_iso = "NO_NTP";

	logEntry["id_log"] = String(now_unix) + "-" + compPatrimonio;
	logEntry["timestamp"] = timestamp_iso;
	logEntry["acao"] = acao;
	logEntry["item_patrimonio"] = compPatrimonio;
	logEntry["item_descricao"] = compDescricao;
	logEntry["uuid_item_tag"] = compUUID;
	logEntry["responsavel_uuid"] = alunoUUID;
	logEntry["responsavel_nome"] = alunoNome;
	logEntry["responsavel_matricula"] = alunoMatricula;
	logEntry["status_item_apos_acao"] = (acao == "emprestimo") ? "emprestado" : "disponivel";

	appendToJsonArray(LOGS_FILE, logEntry);
}


// =================================================================
// MARK: - FUNÇÕES DO LEITOR NFC
// =================================================================

/**
 * @brief Tenta ler o UID de uma tag NFC próxima.
 * @return String com o UID em formato hexadecimal, ou uma string vazia se não houver leitura.
 */
String readNFCUID() {
	if (!pn532_initialized) return "";
	
    uint8_t success;
	uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
	uint8_t uidLength;
    
    // Tenta ler uma tag passiva (padrão MIFARE) com um timeout de 50ms
	success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);

	if (success && uidLength > 0) {
		String uidStr = "";
		for (uint8_t i = 0; i < uidLength; i++) {
			if (uid[i] < 0x10) uidStr += "0"; // Adiciona zero à esquerda para formatação
			uidStr += String(uid[i], HEX);
		}
		uidStr.toUpperCase();
        if (uidStr != ultimoUidNFC) {
            tocarMelodiaLeitura();
            ultimoUidNFC = uidStr;
        }
		return uidStr;
	}
	return "";
}

/**
 * @brief Processa leituras de TAG no modo de operação contínua.
 */
void processarLeituraContinua() {
    String uid = readNFCUID();
    if (uid.length() == 0) return;

    String nome, matricula, patrimonio, descricao;

    
    if (getAlunoInfoByUUID(uid, nome, matricula)) {
        if(estadoOperacao != EstadoOperacao::AGUARDANDO_ALUNO && estadoOperacao != EstadoOperacao::OCIOSO) {
             alunoUUID_sessao = uid;
             alunoNome_sessao = nome;
             Serial.println("CONTINUO: Aluno ATUALIZADO para: " + nome);
        } else {
             alunoUUID_sessao = uid;
             alunoNome_sessao = nome;
             estadoOperacao = EstadoOperacao::AGUARDANDO_COMPUTADOR;
             Serial.println("CONTINUO: Aluno lido: " + nome + ". Aguardando computador...");
        }
    } else if (getComputadorInfoByUUID(uid, patrimonio, descricao)) {
        if(estadoOperacao == EstadoOperacao::AGUARDANDO_COMPUTADOR) {
            Serial.println("CONTINUO: Computador lido: " + patrimonio + ". Processando...");
            ultimoResultado_sessao = executarTransacao(alunoUUID_sessao, uid);
            alunoUUID_sessao = "";
            alunoNome_sessao = "";
            estadoOperacao = EstadoOperacao::AGUARDANDO_ALUNO;
            delay(2000); 
            sinalizarStandby();
            Serial.println("CONTINUO: Transacao finalizada. Aguardando proximo aluno.");
        } else {
            ultimoResultado_sessao = "ERRO: Leia primeiro a TAG de um aluno.";
            sinalizarErro();
            delay(1000);
            sinalizarStandby();
        }
    } else {
        ultimoResultado_sessao = "ERRO: TAG com UUID " + uid + " nao reconhecida.";
        sinalizarErro(); 
        delay(1000);
        sinalizarStandby();
    }
}


// =================================================================
// MARK: - CONFIGURAÇÃO DO SERVIDOR WEB E HANDLERS DA API
// =================================================================

/**
 * @brief Mapeia todas as rotas da API para suas respectivas funções de manipulação.
 */
void configureServerRoutes() {
    // Rotas para servir arquivos estáticos (HTML, CSS)
    httpServer.on("/", HTTP_GET, []() { httpServer.send_P(200, "text/html", INDEX_HTML); });
    httpServer.on("/css/style.css", HTTP_GET, []() { httpServer.send_P(200, "text/css", STYLE_CSS); });
    
    // Rotas para obter listas de dados (GET)
    httpServer.on("/api/alunos", HTTP_GET, [](){ serveFile(ALUNOS_FILE, "application/json"); });
    httpServer.on("/api/computadores", HTTP_GET, [](){ serveFile(COMPUTADORES_FILE, "application/json"); });
    httpServer.on("/api/logs", HTTP_GET, [](){ serveFile(LOGS_FILE, "application/json"); });
    httpServer.on("/api/emprestimos_ativos", HTTP_GET, [](){ serveFile(EMPRESTIMOS_ATIVOS_FILE, "application/json"); });
    
    // Rotas CRUD para Alunos
	httpServer.on("/api/aluno", HTTP_GET, handleApiGetAluno);
	httpServer.on("/api/add_aluno", HTTP_POST, handleApiAddAluno);
	httpServer.on("/api/update_aluno", HTTP_PUT, handleApiUpdateAluno);
	httpServer.on("/api/delete_aluno", HTTP_DELETE, handleApiDeleteAluno);
    
    // Rotas CRUD para Computadores
	httpServer.on("/api/computador", HTTP_GET, handleApiGetComputador);
	httpServer.on("/api/add_computador", HTTP_POST, handleApiAddComputador);
	httpServer.on("/api/update_computador", HTTP_PUT, handleApiUpdateComputador);
	httpServer.on("/api/delete_computador", HTTP_DELETE, handleApiDeleteComputador);

    // Rotas para controle da leitura NFC e operação
	httpServer.on("/api/iniciar_sessao_nfc", HTTP_GET, handleApiIniciarSessaoNFC);
	httpServer.on("/api/get_nfc_tag", HTTP_GET, handleApiGetNFCTag);
	httpServer.on("/api/cancelar_sessao_nfc", HTTP_GET, handleApiCancelarSessaoNFC);
    httpServer.on("/api/toggle_operacao", HTTP_POST, handleToggleOperacaoContinua);
    httpServer.on("/api/status_operacao", HTTP_GET, handleApiGetStatusOperacao);

    // Handler para rotas não encontradas (404)
	httpServer.onNotFound(handleNotFound);
}

void handleNotFound() { 
    httpServer.send(404, "text/plain", "404: Not Found"); 
}

/**
 * @brief Serve um arquivo estático do sistema de arquivos.
 */
void serveFile(const char* filename, const char* contentType){
    if (LittleFS.exists(filename)) {
		File file = LittleFS.open(filename, "r");
		httpServer.streamFile(file, contentType);
		file.close();
	} else {
        httpServer.send(200, contentType, "[]"); // Retorna um array vazio se o arquivo não existe
    }
}

// --- Handlers específicos para Alunos ---

void handleApiGetAluno() {
	String uuid = httpServer.arg("uuid");
	if (uuid == "") { httpServer.send(400, "application/json", "{\"error\":\"UUID ausente\"}"); return; }
	DynamicJsonDocument doc(512); JsonObject record = doc.to<JsonObject>();
	if (getRecordFromJsonArray(ALUNOS_FILE, "uuid_carteirinha", uuid, record)) {
		String output; serializeJson(record, output);
		httpServer.send(200, "application/json", output);
	} else { httpServer.send(404, "application/json", "{\"error\":\"Aluno nao encontrado\"}"); }
}

void handleApiAddAluno() {
	if (!httpServer.hasArg("plain")) { httpServer.send(400, "application/json", "{\"error\":\"Corpo vazio\"}"); return; }
	DynamicJsonDocument doc(512);
	if (deserializeJson(doc, httpServer.arg("plain"))) { httpServer.send(400, "application/json", "{\"error\":\"JSON invalido\"}"); return; }
    JsonObject aluno = doc.as<JsonObject>();
	if (!aluno.containsKey("nome") || !aluno.containsKey("matricula") || !aluno.containsKey("uuid_carteirinha")) {
		httpServer.send(400,"application/json","{\"error\":\"Dados incompletos\"}"); return;
	}
    if (aluno["uuid_carteirinha"].as<String>().length() == 0) { httpServer.send(400,"application/json","{\"error\":\"UUID da carteirinha nao pode ser vazio.\"}"); return; }
	DynamicJsonDocument tempDoc(128); JsonObject dummy = tempDoc.to<JsonObject>();
	if(getRecordFromJsonArray(ALUNOS_FILE, "uuid_carteirinha", aluno["uuid_carteirinha"].as<String>(), dummy)){
		httpServer.send(409,"application/json","{\"error\":\"UUID de carteirinha ja existe.\"}"); return;
	}
	if (appendToJsonArray(ALUNOS_FILE, aluno)) { httpServer.send(201,"application/json","{\"message\":\"Aluno adicionado!\"}");
	} else { httpServer.send(500,"application/json","{\"error\":\"Falha ao guardar aluno.\"}"); }
}

void handleApiUpdateAluno() {
    if (!httpServer.hasArg("plain")) { httpServer.send(400, "application/json", "{\"error\":\"Corpo vazio\"}"); return; }
    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, httpServer.arg("plain"))) { httpServer.send(400, "application/json", "{\"error\":\"JSON invalido\"}"); return; }
    JsonObject alunoData = doc.as<JsonObject>();
    if (!alunoData.containsKey("uuid_original")) { httpServer.send(400, "application/json", "{\"error\":\"uuid_original ausente\"}"); return; }
    String uuidToUpdate = alunoData["uuid_original"].as<String>();
    
    String novoUUID = alunoData["uuid_carteirinha"].as<String>();
    if (uuidToUpdate != novoUUID) {
        DynamicJsonDocument tempDoc(128); JsonObject dummy = tempDoc.to<JsonObject>();
        if(getRecordFromJsonArray(ALUNOS_FILE, "uuid_carteirinha", novoUUID, dummy)){
            httpServer.send(409,"application/json","{\"error\":\"O novo UUID de carteirinha ja esta em uso.\"}"); return;
        }
    }
    
    alunoData.remove("uuid_original");
    if (updateRecordInJsonArray(ALUNOS_FILE, "uuid_carteirinha", uuidToUpdate, alunoData)) { httpServer.send(200, "application/json", "{\"message\":\"Aluno atualizado!\"}");
    } else { httpServer.send(404, "application/json", "{\"error\":\"Aluno nao encontrado.\"}"); }
}

void handleApiDeleteAluno() {
	String uuid = httpServer.arg("uuid");
	if (uuid == "") { httpServer.send(400, "application/json", "{\"error\":\"UUID ausente\"}"); return; }
    String dummyCompUUID, dummyTimestamp;
	if(isAlunoComEmprestimo(uuid, dummyCompUUID, dummyTimestamp)){ httpServer.send(409, "application/json", "{\"error\":\"Aluno possui emprestimo ativo.\"}"); return; }
	if (deleteRecordFromJsonArray(ALUNOS_FILE, "uuid_carteirinha", uuid)) { httpServer.send(200, "application/json", "{\"message\":\"Aluno deletado!\"}");
	} else { httpServer.send(404, "application/json", "{\"error\":\"Aluno nao encontrado.\"}"); }
}

// --- Handlers específicos para Computadores ---

void handleApiGetComputador() {
    String uuid = httpServer.arg("uuid");
    if (uuid == "") { httpServer.send(400, "application/json", "{\"error\":\"UUID ausente\"}"); return; }
    DynamicJsonDocument doc(512); JsonObject record = doc.to<JsonObject>();
    if (getRecordFromJsonArray(COMPUTADORES_FILE, "uuid_tag", uuid, record)) {
        String output; serializeJson(record, output);
        httpServer.send(200, "application/json", output);
    } else { httpServer.send(404, "application/json", "{\"error\":\"Computador nao encontrado\"}"); }
}

void handleApiAddComputador() {
	if (!httpServer.hasArg("plain")) { httpServer.send(400, "application/json", "{\"error\":\"Corpo vazio\"}"); return; }
	DynamicJsonDocument doc(512);
	if (deserializeJson(doc, httpServer.arg("plain"))) { httpServer.send(400, "application/json", "{\"error\":\"JSON invalido\"}"); return; }
    JsonObject comp = doc.as<JsonObject>();
	if (!comp.containsKey("patrimonio")||!comp.containsKey("descricao")||!comp.containsKey("uuid_tag")) {
		httpServer.send(400,"application/json","{\"error\":\"Dados incompletos\"}"); return;
	}
    if (comp["uuid_tag"].as<String>().length() == 0) { httpServer.send(400,"application/json","{\"error\":\"UUID da TAG nao pode ser vazio.\"}"); return; }
	DynamicJsonDocument tempDoc(128); JsonObject dummy = tempDoc.to<JsonObject>();
	if(getRecordFromJsonArray(COMPUTADORES_FILE, "uuid_tag", comp["uuid_tag"].as<String>(), dummy) || 
       getRecordFromJsonArray(COMPUTADORES_FILE, "patrimonio", comp["patrimonio"].as<String>(), dummy)){
		httpServer.send(409,"application/json","{\"error\":\"UUID de tag ou patrimonio ja existe.\"}"); return;
	}
	if (appendToJsonArray(COMPUTADORES_FILE,comp)) { httpServer.send(201,"application/json","{\"message\":\"Computador adicionado!\"}");
    } else { httpServer.send(500,"application/json","{\"error\":\"Falha ao guardar.\"}"); }
}

void handleApiUpdateComputador() {
    if (!httpServer.hasArg("plain")) { httpServer.send(400, "application/json", "{\"error\":\"Corpo vazio\"}"); return; }
	DynamicJsonDocument doc(512);
	if (deserializeJson(doc, httpServer.arg("plain"))) { httpServer.send(400, "application/json", "{\"error\":\"JSON invalido\"}"); return; }
    JsonObject compData = doc.as<JsonObject>();
    if (!compData.containsKey("uuid_original")) { httpServer.send(400, "application/json", "{\"error\":\"uuid_original ausente\"}"); return; }
    String uuidToUpdate = compData["uuid_original"].as<String>();
    
    String novoUUID = compData["uuid_tag"].as<String>();
    if (uuidToUpdate != novoUUID) {
      DynamicJsonDocument tempDoc(128); JsonObject dummy = tempDoc.to<JsonObject>();
      if(getRecordFromJsonArray(COMPUTADORES_FILE, "uuid_tag", novoUUID, dummy)){
          httpServer.send(409,"application/json","{\"error\":\"O novo UUID de TAG ja esta em uso.\"}"); return;
      }
    }
    
    compData.remove("uuid_original");
    if (updateRecordInJsonArray(COMPUTADORES_FILE, "uuid_tag", uuidToUpdate, compData)) { httpServer.send(200, "application/json", "{\"message\":\"Computador atualizado!\"}");
    } else { httpServer.send(404, "application/json", "{\"error\":\"Computador nao encontrado.\"}"); }
}

void handleApiDeleteComputador() {
    String uuid = httpServer.arg("uuid");
    if (uuid == "") { httpServer.send(400, "application/json", "{\"error\":\"UUID ausente\"}"); return; }
    String dummyAlunoUUID;
    if(isComputadorEmprestado(uuid, dummyAlunoUUID)){ httpServer.send(409, "application/json", "{\"error\":\"Computador esta emprestado.\"}"); return; }
    if (deleteRecordFromJsonArray(COMPUTADORES_FILE, "uuid_tag", uuid)) { httpServer.send(200, "application/json", "{\"message\":\"Computador deletado!\"}");
    } else { httpServer.send(404, "application/json", "{\"error\":\"Computador nao encontrado.\"}"); }
}

// --- Handlers da Sessão de Leitura NFC e Operação ---

void handleApiIniciarSessaoNFC() {
	if (!pn532_initialized) { httpServer.send(503, "application/json", "{\"error\":\"Leitor NFC inoperante.\"}"); return; }
	nfcUidParaPolling = "";
	aguardandoLeituraPolling = true;
    lastNFCReadAttempt = millis();
	httpServer.send(200, "application/json", "{\"status\":\"NFC_SCAN_ACTIVATED\"}");
}

void handleApiGetNFCTag() {
	if (!aguardandoLeituraPolling) { 
		httpServer.send(400, "application/json", "{\"error\":\"Leitura nao ativa.\"}"); 
		return; 
	}

	if (nfcUidParaPolling != "") {
		String tipoIdentificado = "desconhecido";
		String nome, patrimonio, descricao, matricula;
		String uuidTemp = nfcUidParaPolling;
		
		if (getAlunoInfoByUUID(uuidTemp, nome, matricula)) {
            tipoIdentificado = "aluno";
        }
		else if (getComputadorInfoByUUID(uuidTemp, patrimonio, descricao)) {
            tipoIdentificado = "computador";
        } 
            
		DynamicJsonDocument doc(512);
		doc["uuid_lida"] = uuidTemp;
		doc["tipo"] = tipoIdentificado;
		if (tipoIdentificado == "aluno") { doc["nome"] = nome; doc["matricula"] = matricula; }
		if (tipoIdentificado == "computador") { doc["patrimonio"] = patrimonio; doc["descricao"] = descricao; }
		
		String output; 
		serializeJson(doc, output);
		httpServer.send(200, "application/json", output);


		nfcUidParaPolling = "";
		aguardandoLeituraPolling = false; 

	} else { 
		httpServer.send(200, "application/json", "{\"uuid_lida\":null}"); 
	}
}

void handleApiCancelarSessaoNFC() {
	aguardandoLeituraPolling = false;
    nfcUidParaPolling = "";
	httpServer.send(200, "application/json", "{\"status\":\"sessao_nfc_cancelada\"}");
}

void handleToggleOperacaoContinua() {
    if (!httpServer.hasArg("plain")) { httpServer.send(400, "application/json", "{\"error\":\"Corpo vazio\"}"); return; }
	DynamicJsonDocument doc(64);
	if (deserializeJson(doc, httpServer.arg("plain"))) { httpServer.send(400, "application/json", "{\"error\":\"JSON invalido\"}"); return; }

    modoOperacaoContinua = doc["ativo"];
    if(modoOperacaoContinua) {
        Serial.println("MODO DE OPERAÇÃO CONTÍNUA: ATIVADO");
        estadoOperacao = EstadoOperacao::AGUARDANDO_ALUNO;
        alunoUUID_sessao = "";
        alunoNome_sessao = "";
        ultimoResultado_sessao = "";
        sinalizarStandby(); 
    } else {
        Serial.println("MODO DE OPERAÇÃO CONTÍNUA: DESATIVADO");
        estadoOperacao = EstadoOperacao::OCIOSO;
    }
    httpServer.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleApiGetStatusOperacao() {
    DynamicJsonDocument doc(512);
    doc["operacaoAtiva"] = modoOperacaoContinua;
    String estadoStr = "Inativo";
    if (modoOperacaoContinua) {
        switch(estadoOperacao) {
            case EstadoOperacao::AGUARDANDO_ALUNO: estadoStr = "Aguardando TAG de Aluno..."; break;
            case EstadoOperacao::AGUARDANDO_COMPUTADOR: estadoStr = "Aguardando TAG de Computador..."; break;
            default: estadoStr = "Ocioso";
        }
    }
    doc["estadoAtual"] = estadoStr;
    doc["alunoSessao"] = alunoNome_sessao;
    doc["toastMessage"] = ultimoResultado_sessao;

    String output;
    serializeJson(doc, output);
    httpServer.send(200, "application/json", output);

    if(ultimoResultado_sessao != "") {
        ultimoResultado_sessao = "";
    }
}


// =================================================================
// MARK: - FUNÇÕES PRINCIPAIS (SETUP E LOOP) - Definições
// =================================================================
// Esta seção está no final para garantir que todas as outras funções
// já tenham sido declaradas antes de serem chamadas.

/**
 * @brief Função de configuração inicial. É executada uma vez quando o ESP32 é ligado ou resetado.
 */
void setup() {
	Serial.begin(115200);
	while (!Serial && millis() < 2000) { }

	Serial.println("\nIniciando Sistema de Gestão de Ativos (SGA)...");
	initFeedbackPins();
	sinalizarStandby(); 
	initLittleFS();
	initWiFi();
	initPN532();

	configureServerRoutes();
	httpServer.begin();
	Serial.println("Servidor HTTP iniciado.");

	if (WiFi.status() == WL_CONNECTED) {
		Serial.print("Acesse a interface em: http://");
		Serial.println(WiFi.localIP());
	} else {
		Serial.println("ATENÇÃO: Wi-Fi não conectado. Interface web inacessível.");
	}
}

/**
 * @brief Loop principal do programa. É executado repetidamente.
 */
void loop() {
	// Processa requisições HTTP recebidas
    if (primeiraVezligado) {
        tocarMelodiaInicializacao();
        primeiraVezligado = false;
    }
	httpServer.handleClient();

    // Se o modo de operação contínua estiver ativo, processa as leituras de TAG
    if (modoOperacaoContinua) {
        processarLeituraContinua();
    } 
    // Se o modo de leitura para formulário estiver ativo E nenhuma tag foi lida ainda
    else if (aguardandoLeituraPolling && nfcUidParaPolling == "") {
        // Verifica se já passou o intervalo para a próxima leitura
		if (millis() - lastNFCReadAttempt > nfcReadInterval) {
			lastNFCReadAttempt = millis();
			String uid = readNFCUID();
			if (uid.length() > 0) {
                // Armazena o UID e para de procurar. O front-end irá coletar este valor.
				nfcUidParaPolling = uid;
			}
		}
	}

	delay(10); 
}


// =================================================================
// MARK: - FUNÇÕES DE INICIALIZAÇÃO - Definições
// =================================================================

/**
 * @brief Inicializa o sistema de arquivos LittleFS. Cria arquivos JSON básicos se não existirem.
 */
void initLittleFS() {
	if (!LittleFS.begin(true)) {
		Serial.println("ERRO CRÍTICO: Falha ao montar o sistema de arquivos LittleFS.");
		while (true) {} // Trava a execução
	}
	Serial.println("Sistema de arquivos LittleFS inicializado.");

	const char* filesToCheck[] = { ALUNOS_FILE, COMPUTADORES_FILE, LOGS_FILE, EMPRESTIMOS_ATIVOS_FILE };
	for (const char* filename : filesToCheck) {
		if (!LittleFS.exists(filename)) {
			File file = LittleFS.open(filename, "w");
			if (file) {
				file.print("[]"); // Cria um array JSON vazio
				file.close();
				Serial.print("Arquivo '"); Serial.print(filename); Serial.println("' criado.");
			} else {
				Serial.print("Falha ao criar o arquivo '"); Serial.println(filename);
			}
		}
	}
}

/**
 * @brief Tenta conectar-se a uma das redes Wi-Fi pré-configuradas.
 */
void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    for (int i = 0; i < numWifiNetworks; i++) {
        Serial.print("Tentando conectar a: ");
        Serial.println(wifiNetworks[i].ssid);
        WiFi.begin(wifiNetworks[i].ssid, wifiNetworks[i].password);

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) { 
            delay(500);
            Serial.print(".");
            attempts++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nWI-FI CONECTADO!");
            Serial.print("Endereço IP: ");
            Serial.println(WiFi.localIP());

            Serial.println("Configurando NTP...");
            configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

            Serial.print("Sincronizando hora com o servidor NTP");
            struct tm timeinfo;
            if (!getLocalTime(&timeinfo, 10000)) { 
                Serial.println("\nERRO CRÍTICO: Falha ao obter a hora do NTP após 10 segundos.");
                sinalizarErro();
                return; 
            }
            
            Serial.println("\nHora sincronizada com sucesso!");
            char time_str[30];
            strftime(time_str, sizeof(time_str), "%A, %B %d %Y %H:%M:%S", &timeinfo);
            Serial.print("Hora atual: ");
            Serial.println(time_str);
            sinalizarStandby();
            return; 
        }
        Serial.println("\nFalha ao conectar.");
    }
    Serial.println("\nNÃO FOI POSSÍVEL CONECTAR A NENHUMA REDE WI-FI.");
    sinalizarErro();
}

/**
 * @brief Inicializa o leitor NFC (PN532) via I2C.
 */
void initPN532() {
	Serial.println("Inicializando leitor NFC PN532 via I2C...");
	Wire.begin(PN532_SDA_PIN, PN532_SCL_PIN);
	
	nfc.begin();

	uint32_t versiondata = nfc.getFirmwareVersion();
	if (!versiondata) {
		Serial.println("ERRO FATAL: Leitor PN532 não encontrado!");
		pn532_initialized = false;
		sinalizarErro(); // Indica falha crítica
	} else {
		Serial.print("Leitor PN532 encontrado. Versão: ");
		Serial.print((versiondata >> 24) & 0xFF, HEX); Serial.print(".");
		Serial.print((versiondata >> 16) & 0xFF, HEX); Serial.print(".");
		Serial.println((versiondata >> 8) & 0xFF, HEX);
		nfc.SAMConfig(); // Configura o módulo de segurança
		Serial.println("PN532 configurado e pronto.");
		pn532_initialized = true;
	}
}

/**
 * @brief Configura os pinos de LED e buzzer como saídas.
 */
void initFeedbackPins() {
	if (LED_VERDE_PIN >= 0) pinMode(LED_VERDE_PIN, OUTPUT);
	if (LED_VERMELHO_PIN >= 0) pinMode(LED_VERMELHO_PIN, OUTPUT);
	if (LED_AMARELO_PIN >= 0) pinMode(LED_AMARELO_PIN, OUTPUT);
    ledcAttach(BUZZER_PIN, PWM_FREQ, PWM_RESOLUTION);
}
