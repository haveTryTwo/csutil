// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/rpc_client.h"

#include <assert.h>
#include <stdio.h>

#include <string>

#include "base/status.h"
#include "sock/demo_book/book_common.h"
#include "sock/demo_book/proto/book.pb.h"

/**
 * @brief 打印 GetBookResp 的内容
 */
static void PrintGetResp(const book_mgr::GetBookResp &get_resp) { /*{{{*/
  fprintf(stderr, "  [get] ret_code=%d, ret_msg=%s, from_cache=%s\n", get_resp.base().ret_code(),
          get_resp.base().ret_msg().c_str(), get_resp.from_cache() ? "true" : "false");
  if (get_resp.base().ret_code() == book_mgr::kBookRetOk && get_resp.has_book()) {
    const book_mgr::Book &b = get_resp.book();
    fprintf(stderr, "        book{id=%s, title=%s, author=%s, price=%d, stock=%d, category=%s}\n",
            b.book_id().c_str(), b.title().c_str(), b.author().c_str(), b.price(), b.stock(),
            b.category().c_str());
  }
} /*}}}*/

/**
 * @brief 构造一本图书
 */
static book_mgr::Book MakeBook(const std::string &id, const std::string &title, const std::string &author,
                               int32_t price, int32_t stock, const std::string &category) { /*{{{*/
  book_mgr::Book book;
  book.set_book_id(id);
  book.set_title(title);
  book.set_author(author);
  book.set_price(price);
  book.set_stock(stock);
  book.set_category(category);
  return book;
} /*}}}*/

int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  std::string ip("127.0.0.1");
  uint16_t port = 9101;
  if (argc >= 3) {
    ip = argv[1];
    port = (uint16_t)atoi(argv[2]);
  }
  fprintf(stderr, "Connect to control server <%s:%d>\n\n", ip.c_str(), port);

  const std::string book_id = "book_1001";

  // ===== 1. Create：写入一本图书 =====
  {
    fprintf(stderr, "[1] CREATE book_id=%s\n", book_id.c_str());
    RpcClient client(ip, port);
    Code ret = client.Init();
    assert(ret == kOk);

    book_mgr::Book book = MakeBook(book_id, "The C++ Programming Language", "Bjarne Stroustrup", 9900, 10,
                                   "programming");
    book_mgr::BookReq req = book_mgr::MakeCreateReq(book);
    book_mgr::BookResp resp;
    ret = client.SendAndRecv(req, &resp);
    assert(ret == kOk);
    fprintf(stderr, "  [create] ret_code=%d, ret_msg=%s, book_id=%s\n\n", resp.create_resp().base().ret_code(),
            resp.create_resp().base().ret_msg().c_str(), resp.create_resp().book_id().c_str());
  }

  // ===== 2. Read：首次查询（缓存未命中 -> 回源 LevelDB -> 回写缓存）=====
  {
    fprintf(stderr, "[2] READ (expect from_cache=false, miss then backfill)\n");
    RpcClient client(ip, port);
    Code ret = client.Init();
    assert(ret == kOk);

    book_mgr::BookReq req = book_mgr::MakeGetReq(book_id);
    book_mgr::BookResp resp;
    ret = client.SendAndRecv(req, &resp);
    assert(ret == kOk);
    PrintGetResp(resp.get_resp());
    fprintf(stderr, "\n");
  }

  // ===== 3. Read：再次查询（缓存命中，from_cache=true）=====
  {
    fprintf(stderr, "[3] READ again (expect from_cache=true)\n");
    RpcClient client(ip, port);
    Code ret = client.Init();
    assert(ret == kOk);

    book_mgr::BookReq req = book_mgr::MakeGetReq(book_id);
    book_mgr::BookResp resp;
    ret = client.SendAndRecv(req, &resp);
    assert(ret == kOk);
    PrintGetResp(resp.get_resp());
    fprintf(stderr, "\n");
  }

  // ===== 4. Update：更新价格与库存，并使缓存失效 =====
  {
    fprintf(stderr, "[4] UPDATE price=8800, stock=5 (invalidate cache)\n");
    RpcClient client(ip, port);
    Code ret = client.Init();
    assert(ret == kOk);

    book_mgr::Book book = MakeBook(book_id, "The C++ Programming Language", "Bjarne Stroustrup", 8800, 5,
                                   "programming");
    book_mgr::BookReq req = book_mgr::MakeUpdateReq(book);
    book_mgr::BookResp resp;
    ret = client.SendAndRecv(req, &resp);
    assert(ret == kOk);
    fprintf(stderr, "  [update] ret_code=%d, ret_msg=%s\n\n", resp.update_resp().base().ret_code(),
            resp.update_resp().base().ret_msg().c_str());
  }

  // ===== 5. Read：更新后查询（缓存已失效 -> 回源取到新值）=====
  {
    fprintf(stderr, "[5] READ after update (expect from_cache=false, new price=8800)\n");
    RpcClient client(ip, port);
    Code ret = client.Init();
    assert(ret == kOk);

    book_mgr::BookReq req = book_mgr::MakeGetReq(book_id);
    book_mgr::BookResp resp;
    ret = client.SendAndRecv(req, &resp);
    assert(ret == kOk);
    PrintGetResp(resp.get_resp());
    fprintf(stderr, "\n");
  }

  // ===== 6. Delete：删除图书并使缓存失效 =====
  {
    fprintf(stderr, "[6] DELETE book_id=%s\n", book_id.c_str());
    RpcClient client(ip, port);
    Code ret = client.Init();
    assert(ret == kOk);

    book_mgr::BookReq req = book_mgr::MakeDeleteReq(book_id);
    book_mgr::BookResp resp;
    ret = client.SendAndRecv(req, &resp);
    assert(ret == kOk);
    fprintf(stderr, "  [delete] ret_code=%d, ret_msg=%s\n\n", resp.delete_resp().base().ret_code(),
            resp.delete_resp().base().ret_msg().c_str());
  }

  // ===== 7. Read：删除后查询（预期 not found）=====
  {
    fprintf(stderr, "[7] READ after delete (expect not found)\n");
    RpcClient client(ip, port);
    Code ret = client.Init();
    assert(ret == kOk);

    book_mgr::BookReq req = book_mgr::MakeGetReq(book_id);
    book_mgr::BookResp resp;
    ret = client.SendAndRecv(req, &resp);
    assert(ret == kOk);
    PrintGetResp(resp.get_resp());
    fprintf(stderr, "\n");
  }

  fprintf(stderr, "All CRUD operations done.\n");
  return 0;
} /*}}}*/
