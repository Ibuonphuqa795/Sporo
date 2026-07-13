# ESP32 Cloud Dashboard — Hướng dẫn sử dụng

Hệ thống gồm 3 phần:

```
ESP32 (testpercen.ino)  →  gửi GET /api/update?temp=..&moisture=..  →  server.js (Express)  →  web dashboard (public/index.html)
```

`server.js` cũ của bạn bị hỏng (chứa lỗi "429 Too Many Requests" của GitHub, không phải code). File đã được viết lại đầy đủ.

---

## A. Chạy trên máy tính (localhost) — để test nhanh

1. Mở terminal tại thư mục `esp32`, chạy:

   ```bash
   npm install     # chỉ cần làm 1 lần
   npm start
   ```

2. Mở trình duyệt: **http://localhost:3000** → thấy dashboard.

3. Test gửi dữ liệu giả (mở tab terminal khác):

   ```bash
   curl "http://localhost:3000/api/update?temp=28.5&moisture=65"
   ```

   Dashboard sẽ tự cập nhật sau vài giây.

### Để ESP32 gửi về máy này (cùng mạng WiFi)

Sửa trong `testpercen.ino`, đổi dòng `cloudServer` thành IP máy tính của bạn:

```cpp
const char* cloudServer = "http://192.168.1.15:3000/api/update";
```

> Xem IP máy tính: Windows dùng `ipconfig`, Mac/Linux dùng `ifconfig` hoặc `ip a`.
> ESP32 và máy tính phải chung một mạng WiFi.

---

## B. Deploy lên Render.com — để xem từ mọi nơi

Link trong ESP32 hiện là `https://esp32-plant.onrender.com/api/update`, nên deploy lên Render là đúng nhất.

1. Đẩy code (`server.js`, `package.json`, thư mục `public/`) lên một repo GitHub.

2. Vào https://render.com → **New** → **Web Service** → chọn repo đó.

3. Cấu hình:
   - **Build Command:** `npm install`
   - **Start Command:** `npm start`
   - **Instance Type:** Free

4. Render tự cấp URL. Nếu URL khác `esp32-plant`, sửa lại `cloudServer` trong ESP32 cho khớp.

5. Xong: mở URL Render trên điện thoại/máy tính bất kỳ để xem dashboard.

> **Lưu ý gói Free của Render:** server "ngủ" sau 15 phút không ai truy cập. Request đầu tiên sau khi ngủ sẽ chậm ~30 giây. ESP32 gửi mỗi 20 giây (đã set trong code) nên thường giữ server thức.

---

## Ghi chú kỹ thuật

- **Dữ liệu lưu trong RAM**, không dùng database. Restart server → mất lịch sử. Đủ dùng cho đồ án; nếu cần lưu lâu dài thì thêm database sau.
- `server.js` dùng `process.env.PORT` nên tự chạy đúng cổng trên Render, và cổng 3000 khi ở local — không cần sửa gì.
- Endpoint kiểm tra server sống: `/health` (trả về `OK`).
