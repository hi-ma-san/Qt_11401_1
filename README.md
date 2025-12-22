# Desktop Information Tool (桌面資訊工具)

## 專案簡介 (Project Description)
本專案為「視窗程式設計」課程之期末專題，旨在開發一個類似 Rainmeter 的可客製化桌面資訊工具。
此工具以浮動視窗的形式常駐於桌面，提供使用者即時的系統硬體資訊監控，並支援高度客製化的外觀與互動行為。

## 專案目標 (Project Goals)
- 建立可客製化、浮動於桌面的資訊工具
- 可顯示系統、硬體使用資訊
- 可由使用者自訂位置、顯示模式以及懸停反應

## 功能特色 (Features)

### 1. 視窗與互動行為 (Window & Interaction)
- **無邊框設計 (Frameless)**：視窗無邊框，支援透明背景。
- **顯示模式 (Display Modes)**：
  - 永遠置頂 (Always on Top)
  - 位於桌面 (On Desktop)
- **互動功能 (Interactive Behaviors)**：
  - 拖曳移動 (Drag to move)
  - 點擊穿透 (Click-through)
  - 懸停反應/消失 (Hover reaction/hide)
- **狀態保存**：
  - 設定元件透明度
  - 自動儲存元件擺放位置

### 2. 系統資訊模組 (System Information Modules)
- **CPU 使用率** (CPU Usage)
- **RAM 使用率** (RAM Usage)
- **硬碟容量/使用量** (Disk Capacity/Usage)
- **日期與時間** (Date & Time)
- **網路使用量** (Network Usage)

### 3. 客製化與擴充功能 (Customization & Extras)
- **自訂顯示圖片**：支援 GIF、PNG、JPG 等格式。
- **自訂懸停圖片**：滑鼠懸停時改變顯示圖片。
- **應用程式快捷鍵**：自訂快捷開啟應用程式元件。
- **實用工具**：
  - 待辦事項 (To-Do List)
  - 行事曆 (Calendar)

## 開發環境 (Development Environment)
- **作業系統**: Windows
- **開發工具**: Qt Creator
- **框架版本**: Qt 5.x / Qt 6.x
- **程式語言**: C++