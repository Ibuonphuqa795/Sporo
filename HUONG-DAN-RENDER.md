# Deploy web lên Render.com — Hướng dẫn từng bước

Mục tiêu: đưa code (`server.js`, `package.json`, thư mục `public/`) lên internet để ESP32 gửi dữ liệu và bạn xem dashboard từ mọi nơi.

Render **lấy code từ GitHub**, nên luồng làm là: **GitHub trước → Render sau**.

Bạn KHÔNG cần cài Node.js hay dùng terminal cho việc deploy này — Render tự cài và chạy trên cloud.

---

## PHẦN 1 — Đưa code lên GitHub

### Bước 1. Tạo tài khoản GitHub
1. Vào https://github.com → **Sign up**.
2. Nhập email, mật khẩu, username → xác nhận email.

### Bước 2. Tạo repository mới (kho chứa code)
1. Góc trên phải bấm dấu **+** → **New repository**.
2. **Repository name:** đặt tên, ví dụ `esp32-plant`.
3. Chọn **Public**.
4. **KHÔNG** tick "Add a README".
5. Bấm **Create repository**.

### Bước 3. Tải code lên (cách dễ nhất, không cần terminal)
1. Trong repo vừa tạo, bấm dòng **"uploading an existing file"**.
2. Kéo-thả các file sau vào trang:
   - `server.js`
   - `package.json`
   - **cả thư mục `public`** (kéo nguyên thư mục vào — bên trong có `index.html`)
   - ⚠️ **KHÔNG** tải `node_modules` lên (thư mục này rất nặng và không cần — Render tự cài).
3. Kéo xong bấm nút xanh **Commit changes**.

> Kiểm tra: trong repo phải thấy đủ `server.js`, `package.json`, và `public/index.html`.

---

## PHẦN 2 — Deploy trên Render

### Bước 4. Tạo tài khoản Render
1. Vào https://dashboard.render.com → **Get Started** / **Sign in**.
2. Chọn **Sign in with GitHub** (đăng nhập bằng chính tài khoản GitHub vừa tạo) → **Authorize**.

### Bước 5. Tạo Web Service
1. Trên dashboard bấm **New** (góc trên phải) → chọn **Web Service**.
2. Ở phần chọn repo, tìm repo `esp32-plant`.
   - Nếu không thấy: bấm **Configure GitHub App** → cấp quyền cho Render truy cập repo đó → quay lại.
3. Bấm **Connect** cạnh repo.

### Bước 6. Cấu hình
Điền các ô sau:

| Ô | Giá trị |
|---|---|
| **Name** | `esp32-plant` (đây sẽ thành URL: `esp32-plant.onrender.com`) |
| **Branch** | `main` |
| **Build Command** | `npm install` |
| **Start Command** | `npm start` |
| **Instance Type** | **Free** |

> ⚠️ Nếu tên `esp32-plant` đã bị người khác dùng, Render sẽ cho bạn URL khác (ví dụ `esp32-plant-abc1.onrender.com`). Ghi nhớ URL thật này để sửa lại ESP32 ở Bước 8.

### Bước 7. Deploy
1. Bấm **Create Web Service** (hoặc **Deploy**).
2. Chờ 1–3 phút. Khi log hiện `Server chay tai...` và trạng thái chuyển **Live** (màu xanh) là xong.
3. Bấm vào URL ở đầu trang → thấy dashboard (lúc này hiện "Chưa có dữ liệu" vì ESP32 chưa gửi).

---

## PHẦN 3 — Nối ESP32 vào web

### Bước 8. Sửa link trong ESP32
Mở `testpercen.ino`, kiểm tra dòng này khớp với URL Render thật của bạn:

```cpp
const char* cloudServer = "https://esp32-plant.onrender.com/api/update";
```

Nếu URL Render khác thì sửa lại cho đúng. Sau đó nạp (upload) lại code vào ESP32.

Xong! ESP32 gửi dữ liệu mỗi 20 giây → mở URL Render trên điện thoại là thấy nhiệt độ + độ ẩm cập nhật.

---

## Cập nhật code sau này (khi sửa server.js hay dashboard)

Vì Render nối với GitHub nên **mỗi lần bạn cập nhật file trên GitHub, Render tự deploy lại**:

1. Vào repo trên GitHub → mở file cần sửa → bấm biểu tượng bút chì ✏️ để sửa, hoặc **Add file → Upload files** để thay file mới.
2. Bấm **Commit changes**.
3. Render tự động build lại sau ~1 phút. Không cần làm gì thêm.

---

## Lưu ý gói Free của Render
- Server **"ngủ" sau 15 phút** không có request → request đầu tiên sau đó chậm ~30–50 giây (server thức dậy). ESP32 gửi đều mỗi 20 giây nên thường giữ server thức.
- Dữ liệu lưu trong RAM → **restart/deploy lại sẽ mất lịch sử**. Đủ cho đồ án; cần lưu lâu dài thì thêm database sau.
