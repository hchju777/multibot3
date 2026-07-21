#pragma once

/**
 * @file sim_backend_registry.hpp
 * @brief ISimBackend(seam c) 구현체 선택·보관 — "백엔드 선택은 config 한 줄" 의 코드측 실현.
 *
 * @note 구체 백엔드(FakeSimBackend/pysim 프로세스 어댑터/isaac 어댑터)는 **여기서 파생하지
 * 않는다**. `ISimBackend` 의 가상 메서드는 전부 `noexcept` 이며(계약: "예외를 던지지 않는다 —
 * 실패는 status/bool 로 보고"), noexcept 오버라이드 안에서 `throw` 하면 `-Wterminate` 가
 * `-Werror` 하에 빌드를 깨뜨린다(실측 확인). 따라서 이 스캐폴딩 단계에서는 **구체 어댑터를
 * 만들지 않고** 포트 자리와 이 레지스트리만 둔다 — 구체 구현은 Phase 5(coordination-builder,
 * pysim 프로세스 어댑터) / 진화 단계(mrs_sim_isaac) 의 몫이다. 상세 근거는
 * `03_scaffold/scaffold.md` "noexcept 포트와 throw 스텁의 충돌" 절 참조.
 */

#include <memory>
#include <string>

#include "mrs/i_sim_backend.hpp"

namespace mrs
{

/**
 * @brief 백엔드 이름(config 값)으로 ISimBackend 구현체를 선택·보관하는 레지스트리.
 */
class SimBackendRegistry
{
public:
  SimBackendRegistry() = default;
  ~SimBackendRegistry() = default;

  /**
   * @brief 이름으로 백엔드를 등록한다 ("fake" | "pysim" | "isaac").
   * @param[in] backend_name config 값과 일치하는 이름.
   * @param[in] backend 등록할 구현체.
   * @return void
   */
  void register_backend(const std::string & backend_name, std::shared_ptr<ISimBackend> backend);

  /**
   * @brief 현재 활성 백엔드를 config 값으로 선택한다.
   * @param[in] backend_name "fake" | "pysim" | "isaac".
   * @return bool 선택 성공 여부(미등록 이름이면 false).
   */
  bool select_backend(const std::string & backend_name);

  /**
   * @brief 현재 선택된 백엔드를 반환한다.
   * @return ISimBackend* 관측 포인터. 미선택이면 nullptr.
   */
  ISimBackend * active() const noexcept
  {
    return active_.get();
  }

private:
  std::shared_ptr<ISimBackend> active_; ///< 현재 선택된 백엔드 (seam c)
};

} // namespace mrs
