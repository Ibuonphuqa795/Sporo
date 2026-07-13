# Kết nối Database để lưu tài khoản vĩnh viễn

Mặc định web dùng bộ nhớ RAM → tài khoản đăng ký sẽ **mất khi Render ngủ/deploy lại**.
Để tài khoản **lưu vĩnh viễn**, nối web với một database Postgres miễn phí (Neon).
Code đã viết sẵn: **chỉ cần thêm 1 biến môi trường `DATABASE_URL`** trên Render, không cần sửa code.

> Không làm bước này cũng được — web vẫn chạy, chỉ là tài khoản đăng ký không lưu lâu dài. 6 tài khoản nhóm có sẵn thì luôn dùng được.

---

## Bước 1 — Tạo database miễn phí trên Neon

1. Vào https://neon.tech → **Sign up** (đăng nhập bằng Google/GitHub cho nhanh, miễn phí, không cần thẻ).
2. Tạo **New Project** → đặt tên bất kỳ (vd `sporo`) → chọn region gần (Singapore) → **Create**.
3. Sau khi tạo, bấm nút **Connect** (góc trên) → hiện **Connection string**.
4. **Copy** chuỗi đó. Nó có dạng:

   ```
   postgresql://user:matkhau@ep-xxxx.ap-southeast-1.aws.neon.tech/dbname?sslmode=require
   ```

---

## Bước 2 — Dán vào Render

1. Vào https://dashboard.render.com → mở service **esp32-plant**.
2. Menu trái bấm **Environment**.
3. Bấm **Add Environment Variable**:
   - **Key:** `DATABASE_URL`
   - **Value:** dán chuỗi connection string vừa copy ở Bước 1
4. Bấm **Save Changes**. Render tự deploy lại (~1–2 phút).

Xong! Từ giờ mọi tài khoản đăng ký qua web được lưu vào database, không bao giờ mất.

---

## Kiểm tra đã hoạt động chưa

Sau khi deploy lại, xem **Logs** của service trên Render:
- Thấy dòng `Auth: dùng Postgres (tài khoản lưu vĩnh viễn)` → **thành công**.
- Nếu thấy `Auth: dùng RAM ...` → chưa nhận được `DATABASE_URL`, kiểm tra lại Bước 2.

Thử: đăng ký 1 tài khoản mới → đợi web ngủ (hoặc bấm Manual Deploy) → đăng nhập lại bằng tài khoản đó. Nếu vẫn vào được → database đã lưu thành công.

---

## Chạy thử trên máy với database (tùy chọn)

Nếu muốn test database ngay trên máy:

```bash
# Mac/Linux
DATABASE_URL="postgresql://...chuỗi của bạn..." npm start
```

Không đặt `DATABASE_URL` thì web tự dùng RAM (tài khoản nhóm vẫn đăng nhập được bình thường).

---

## Ghi chú
- 6 tài khoản nhóm (khoa, phuc, hung, quan, tai, admin) tự động được nạp vào database lần đầu chạy, nên vẫn đăng nhập được kể cả sau khi nối DB.
- Mật khẩu được **mã hoá (bcrypt)** trước khi lưu — trong database không thấy mật khẩu gốc, an toàn hơn.
- Neon free đủ dùng cho đồ án (dung lượng và số kết nối rộng rãi).
